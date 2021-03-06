//
// Created by lixiaoqing on 2021/9/8.
//

#include <vector>
#include "SQLiteDatabase.h"


typedef std::map<std::string, SQLiteResultSet *>::iterator CachesIter;


SQLiteDatabase::SQLiteDatabase() {
    _db = nullptr;
    _numberOfRetries = 1;
}


SQLiteDatabase::~SQLiteDatabase() {
    close();
}


bool SQLiteDatabase::open(const char *dbPath) {
    std::string otherPath(dbPath);
    if (otherPath == _dbPath) {
        return false;
    }

    close();
    _dbPath = otherPath;
    int err = sqlite3_open(_dbPath.c_str(), &_db);
    if (err != SQLITE_OK) {
        return false;
    }
    return true;
}


void SQLiteDatabase::uncacheCachesResultSets() {
    for (CachesIter iter = _cacheResultSets.begin(); iter != _cacheResultSets.end(); ++iter) {
        SQLiteResultSet *rc = iter->second;
        rc->_cached = false;
        if (!rc->isOpen()) {
            sqlite3_finalize(rc->_pStmt);
            delete rc;
        }
    }
    _cacheResultSets.clear();
}


void SQLiteDatabase::closeOpenedResultSets() {
    size_t count = _openResultSets.size();
    for (unsigned int idx = 0; idx < count; idx++) {
        SQLiteResultSet *rs = _openResultSets[idx];
        rs->_closeNeedNoticeDb(false);
        if (!rs->_cached) {
            delete rs;
        }
    }
    _openResultSets.clear();

    if (_reuseResultSet.isOpen()) {
        _reuseResultSet._closeNeedNoticeDb(false);
    }
}


void SQLiteDatabase::resultSetDidfinalize(SQLiteResultSet *resultSet) {
    if (resultSet != &_reuseResultSet) {
        size_t count = _openResultSets.size();
        for (unsigned int idx = 0; idx < count; idx++) {
            SQLiteResultSet *rs = _openResultSets[idx];
            if (rs == resultSet) {
                if (!rs->_cached) {
                    delete rs;
                }
                _openResultSets.erase(_openResultSets.begin() + idx);
                break;
            }
        }
    }
}


bool SQLiteDatabase::close() {
    closeOpenedResultSets();
    uncacheCachesResultSets();

    if (!_db) {
        return true;
    }

    bool triedFinalizingOpenStatements = false;
    bool needRetry = true;
    int rc = SQLITE_OK;
    for (int idx = 0; idx <= _numberOfRetries && needRetry; idx++) {
        needRetry = false;
        rc = sqlite3_close(_db);
        if (rc == SQLITE_BUSY || rc == SQLITE_LOCKED) {
            needRetry = true;
            usleep(20);
            if (!triedFinalizingOpenStatements) {
                triedFinalizingOpenStatements = true;
                sqlite3_stmt *pStmt;
                while ((pStmt = sqlite3_next_stmt(_db, 0x00)) != 0) {
                    sqlite3_finalize(pStmt);
                }
            }
        }
    }

    if (rc == SQLITE_OK) {
        _db = nullptr;
        return true;
    }
    return false;
}


bool SQLiteDatabase::cacheSQL(const char *s) {
    std::string sql(s);
    CachesIter iter = _cacheResultSets.find(sql);
    if ((iter != _cacheResultSets.end()) && (iter->first == sql)) {
        return false;
    }

    sqlite3_stmt *pStmt = stmtPrepare(s);
    if (!pStmt) {
        return false;
    }

    SQLiteResultSet *resultSet = new SQLiteResultSet;
    resultSet->_pStmt = pStmt;
    resultSet->_cached = true;
    _cacheResultSets.insert(iter, std::make_pair(sql, resultSet));

    return true;
}


bool SQLiteDatabase::uncacheSQL(const char *s) {
    if (_cacheResultSets.empty()) {
        return false;
    }

    std::string sql(s);
    CachesIter iter = _cacheResultSets.find(sql);
    if ((iter == _cacheResultSets.end()) || (iter->first != sql)) {
        return false;
    }

    SQLiteResultSet *rc = iter->second;
    rc->_cached = false;
    if (!rc->isOpen()) {
        sqlite3_finalize(rc->_pStmt);
        delete rc;
    }
    _cacheResultSets.erase(iter);
    return true;
}


void SQLiteDatabase::bindValue(const SQLiteValue &value, int idx, sqlite3_stmt *stmt) {
    switch (value.type()) {
        case SQLValueType_Null:
            sqlite3_bind_null(stmt, idx);
            break;

        case SQLValueType_Int:
            sqlite3_bind_int(stmt, idx, value.intValue());
            break;

        case SQLValueType_Double:
            sqlite3_bind_double(stmt, idx, value.doubleValue());
            break;

        case SQLValueType_Int64:
            sqlite3_bind_int64(stmt, idx, value.int64Value());
            break;

        case SQLValueType_Text:
            sqlite3_bind_text(stmt, idx, value.textValue(), -1, SQLITE_STATIC);
            break;

        case SQLValueType_Blob:
            sqlite3_bind_blob(stmt, idx, value.blobBytes(), value.blobLength(), SQLITE_STATIC);
            break;

        default:
            break;
    }
}

sqlite3_stmt *SQLiteDatabase::stmtPrepare(const char *sql) {
    if (!databaseExists()) {
        return nullptr;
    }

    sqlite3_stmt *pStmt = nullptr;
    bool needRetry = true;
    int rc = SQLITE_OK;

    for (int idx = 0; idx <= _numberOfRetries && needRetry; idx++) {
        needRetry = false;
        rc = sqlite3_prepare_v2(_db, sql, -1, &pStmt, 0);
        if (SQLITE_BUSY == rc || SQLITE_LOCKED == rc) {
            needRetry = true;
            usleep(20);
        }
    }

    if (rc != SQLITE_OK) {
        sqlite3_finalize(pStmt);
        return nullptr;
    }

    return pStmt;
}


SQLiteResultSet *SQLiteDatabase::findClosedCache(const char *sql) {
    if (_cacheResultSets.empty()) {
        return nullptr;
    }

    std::string key(sql);
    CachesIter iter = _cacheResultSets.find(key);
    if ((iter != _cacheResultSets.end()) && (iter->first == sql)) {
        if (!iter->second->isOpen()) {
            return iter->second;
        }
    }
    return nullptr;
}


void SQLiteDatabase::bindValues(const SQLiteValue *values, int length, sqlite3_stmt *stmt) {
    int queryCount = sqlite3_bind_parameter_count(stmt);
    int count = std::min(queryCount, length);
    for (int idx = 0; idx < count; idx++) {
        bindValue(values[idx], idx + 1, stmt);
    }
}


bool SQLiteDatabase::exec(const char *sql) {
    return exec(sql, nullptr, 0);
}


bool SQLiteDatabase::exec(const char *sql, const SQLiteValue &v0) {
    SQLiteValue values[] = {v0};
    return exec(sql, values, sizeof(values) / sizeof(values[0]));
}


bool SQLiteDatabase::exec(const char *sql, const SQLiteValue &v0, const SQLiteValue &v1) {
    SQLiteValue values[] = {v0, v1};
    return exec(sql, values, sizeof(values) / sizeof(values[0]));
}


bool SQLiteDatabase::exec(const char *sql, const SQLiteValue *values, int length) {
    SQLiteResultSet resultSet;
    SQLiteResultSet *pResultSet = findClosedCache(sql);
    if (!pResultSet) {
        resultSet._pStmt = stmtPrepare(sql);
        pResultSet = &resultSet;
    }
    pResultSet->open(this);

    if (pResultSet->_pStmt == nullptr) {
        return false;
    }

    bindValues(values, length, pResultSet->_pStmt);
    pResultSet->next();
    pResultSet->_closeNeedNoticeDb(false);
    return true;
}


bool SQLiteDatabase::execBatch(const char *sql, const std::vector<SQLiteValue *> &listValues, int length) {
    bool result = true;
    const int size = listValues.size();
    if (size <= 0 || length == 0) {
        return exec(sql);
    }

    SQLiteResultSet resultSet;
    resultSet._pStmt = stmtPrepare("BEGIN EXCLUSIVE;");
    SQLiteResultSet *pResultSet = &resultSet;
    pResultSet->open(this);

    if (pResultSet->_pStmt == nullptr) {
        return false;
    }

    pResultSet->next();

    resultSet._pStmt = stmtPrepare(sql);
    for (int i = 0; i < size; i++) {
        SQLiteValue *values = listValues[i];
        bindValues(values, length, pResultSet->_pStmt);
        result &= pResultSet->step();
        if (nullptr != values) {
            delete[] values;
            values = nullptr;
        }
    }

    resultSet._pStmt = stmtPrepare(result ? "COMMIT;" : "ROLLBACK;");
    pResultSet->next();
    pResultSet->_closeNeedNoticeDb(false);

    return true;
}


SQLiteResultSet *SQLiteDatabase::query(const char *sql) {
    return query(sql, nullptr, 0);
}


SQLiteResultSet *SQLiteDatabase::query(const char *sql, const SQLiteValue &v0) {
    SQLiteValue values[] = {v0};
    return query(sql, values, sizeof(values) / sizeof(values[0]));
}


SQLiteResultSet *SQLiteDatabase::query(const char *sql, const SQLiteValue &v0, const SQLiteValue &v1) {
    SQLiteValue values[] = {v0, v1};
    return query(sql, values, sizeof(values) / sizeof(values[0]));
}


SQLiteResultSet *SQLiteDatabase::query(const char *sql,
                                       const SQLiteValue *values,
                                       int length) {
    SQLiteResultSet *pResultSet = findClosedCache(sql);
    if (pResultSet) {
        _openResultSets.push_back(pResultSet);
    } else if (_reuseResultSet.isOpen()) {
        pResultSet = new SQLiteResultSet();
        pResultSet->_pStmt = stmtPrepare(sql);
        _openResultSets.push_back(pResultSet);
    } else {
        pResultSet = &_reuseResultSet;
        pResultSet->_pStmt = stmtPrepare(sql);
    }

    if (pResultSet->_pStmt == nullptr) {
        return nullptr;
    }

    bindValues(values, length, pResultSet->_pStmt);
    pResultSet->open(this);
    return pResultSet;
}

int SQLiteDatabase::getTableRows(const char *name) const {
    SQLiteDatabase *db = const_cast<SQLiteDatabase *>(this);
    char sql[256];
    sprintf(sql, "select count(*) from %s", name);
    SQLiteResultSet *rs = db->query(sql);

    if (rs) {
        int count = 0;
        if (rs->next()) {
            count = rs->columnInt(0);
        }
        rs->close();
        return count;
    }

    return 0;
}

bool SQLiteDatabase::isTableExists(const char *name) const {
    SQLiteDatabase *db = const_cast<SQLiteDatabase *>(this);
    SQLiteResultSet *rs = db->query("select count(*) from sqlite_master where type='table' and name = ?",
                                    SQLText(name));

    if (rs) {
        rs->next();
        int count = rs->columnInt(0);
        rs->close();
        return (count == 1);
    }

    return 0;
}

// ??????????????????????????????
int SQLiteDatabase::getVersion() const {
    SQLiteDatabase *db = const_cast<SQLiteDatabase *>(this);
    SQLiteResultSet *rs = db->query("PRAGMA user_version;");

    int version = 0;
    if (rs) {
        rs->next();
        version = rs->columnInt(0);
        rs->close();

    }

    return version;
}

void SQLiteDatabase::setVersion(int version) const {
    SQLiteDatabase *db = const_cast<SQLiteDatabase *>(this);

    char sql[128];
    sprintf(sql, "PRAGMA user_version=%d;", version);
    db->exec(sql);
}