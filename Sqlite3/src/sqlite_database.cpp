//
// Created by lixiaoqing on 2021/9/8.
//

#include "sqlite_database.h"

#include <vector>

typedef std::map<std::string, SqliteResultSet *>::iterator CachesIter;


SqliteDatabase::SqliteDatabase() {
    db_ = nullptr;
    number_of_retries_ = 1;
}


SqliteDatabase::~SqliteDatabase() {
    Close();
}


bool SqliteDatabase::Open(const char *db_path) {
    std::string other_path(db_path);
    if (other_path == dbPath_) {
        return false;
    }

    Close();
    dbPath_ = other_path;
    int err = sqlite3_open(dbPath_.c_str(), &db_);
    if (err != SQLITE_OK) {
        return false;
    }
    return true;
}


void SqliteDatabase::UncacheCachesResultSets() {
    for (CachesIter iter = cache_result_sets_.begin(); iter != cache_result_sets_.end(); ++iter) {
        SqliteResultSet *rc = iter->second;
        rc->cached_ = false;
        if (!rc->IsOpen()) {
            sqlite3_finalize(rc->stmt_);
            delete rc;
        }
    }
    cache_result_sets_.clear();
}


void SqliteDatabase::CloseOpenedResultSets() {
    size_t count = open_result_sets_.size();
    for (unsigned int idx = 0; idx < count; idx++) {
        SqliteResultSet *rs = open_result_sets_[idx];
        rs->CloseNeedNoticeDb(false);
        if (!rs->cached_) {
            delete rs;
        }
    }
    open_result_sets_.clear();

    if (reuse_result_set_.IsOpen()) {
        reuse_result_set_.CloseNeedNoticeDb(false);
    }
}


void SqliteDatabase::ResultSetDidfinalize(SqliteResultSet *result_set) {
    if (result_set != &reuse_result_set_) {
        size_t count = open_result_sets_.size();
        for (unsigned int idx = 0; idx < count; idx++) {
            SqliteResultSet *rs = open_result_sets_[idx];
            if (rs == result_set) {
                if (!rs->cached_) {
                    delete rs;
                }
                open_result_sets_.erase(open_result_sets_.begin() + idx);
                break;
            }
        }
    }
}


bool SqliteDatabase::Close() {
    CloseOpenedResultSets();
    UncacheCachesResultSets();

    if (!db_) {
        return true;
    }

    bool tried_finalizing_open_statements = false;
    bool need_retry = true;
    int rc = SQLITE_OK;
    for (int idx = 0; idx <= number_of_retries_ && need_retry; idx++) {
        need_retry = false;
        rc = sqlite3_close(db_);
        if (rc == SQLITE_BUSY || rc == SQLITE_LOCKED) {
            need_retry = true;
            usleep(20);
            if (!tried_finalizing_open_statements) {
                tried_finalizing_open_statements = true;
                sqlite3_stmt *pStmt;
                while ((pStmt = sqlite3_next_stmt(db_, 0x00)) != 0) {
                    sqlite3_finalize(pStmt);
                }
            }
        }
    }

    if (rc == SQLITE_OK) {
        db_ = nullptr;
        return true;
    }
    return false;
}


bool SqliteDatabase::CacheSql(const char *s) {
    std::string sql(s);
    CachesIter iter = cache_result_sets_.find(sql);
    if ((iter != cache_result_sets_.end()) && (iter->first == sql)) {
        return false;
    }

    sqlite3_stmt *pStmt = StmtPrepare(s);
    if (!pStmt) {
        return false;
    }

    SqliteResultSet *resultSet = new SqliteResultSet;
    resultSet->stmt_ = pStmt;
    resultSet->cached_ = true;
    cache_result_sets_.insert(iter, std::make_pair(sql, resultSet));

    return true;
}


bool SqliteDatabase::UncacheSql(const char *s) {
    if (cache_result_sets_.empty()) {
        return false;
    }

    std::string sql(sql);
    CachesIter iter = cache_result_sets_.find(sql);
    if ((iter == cache_result_sets_.end()) || (iter->first != sql)) {
        return false;
    }

    SqliteResultSet *rc = iter->second;
    rc->cached_ = false;
    if (!rc->IsOpen()) {
        sqlite3_finalize(rc->stmt_);
        delete rc;
    }
    cache_result_sets_.erase(iter);
    return true;
}


void SqliteDatabase::BindValue(const SqliteValue &value, int idx, sqlite3_stmt *stmt) {
    switch (value.Type()) {
        case SQLValueType_Null:
            sqlite3_bind_null(stmt, idx);
            break;

        case SQLValueType_Int:
            sqlite3_bind_int(stmt, idx, value.IntValue());
            break;

        case SQLValueType_Double:
            sqlite3_bind_double(stmt, idx, value.DoubleValue());
            break;

        case SQLValueType_Int64:
            sqlite3_bind_int64(stmt, idx, value.Int64Value());
            break;

        case SQLValueType_Text:
            sqlite3_bind_text(stmt, idx, value.TextValue(), -1, SQLITE_STATIC);
            break;

        case SQLValueType_Blob:
            sqlite3_bind_blob(stmt, idx, value.BlobBytes(), value.BlobLength(), SQLITE_STATIC);
            break;

        default:
            break;
    }
}

sqlite3_stmt *SqliteDatabase::StmtPrepare(const char *sql) {
    if (!DatabaseExists()) {
        return nullptr;
    }

    sqlite3_stmt *pStmt = nullptr;
    bool need_retry = true;
    int rc = SQLITE_OK;

    for (int idx = 0; idx <= number_of_retries_ && need_retry; idx++) {
        need_retry = false;
        rc = sqlite3_prepare_v2(db_, sql, -1, &pStmt, 0);
        if (SQLITE_BUSY == rc || SQLITE_LOCKED == rc) {
            need_retry = true;
            usleep(20);
        }
    }

    if (rc != SQLITE_OK) {
        sqlite3_finalize(pStmt);
        return nullptr;
    }

    return pStmt;
}


SqliteResultSet *SqliteDatabase::FindClosedCache(const char *sql) {
    if (cache_result_sets_.empty()) {
        return nullptr;
    }

    std::string key(sql);
    CachesIter iter = cache_result_sets_.find(key);
    if ((iter != cache_result_sets_.end()) && (iter->first == sql)) {
        if (!iter->second->IsOpen()) {
            return iter->second;
        }
    }
    return nullptr;
}


void SqliteDatabase::BindValues(const SqliteValue *values, int length, sqlite3_stmt *stmt) {
    int query_count = sqlite3_bind_parameter_count(stmt);
    int count = std::min(query_count, length);
    for (int idx = 0; idx < count; idx++) {
        BindValue(values[idx], idx + 1, stmt);
    }
}


bool SqliteDatabase::Exec(const char *sql) {
    return Exec(sql, nullptr, 0);
}


bool SqliteDatabase::Exec(const char *sql, const SqliteValue &v0) {
    SqliteValue values[] = {v0};
    return Exec(sql, values, sizeof(values) / sizeof(values[0]));
}


bool SqliteDatabase::Exec(const char *sql, const SqliteValue &v0, const SqliteValue &v1) {
    SqliteValue values[] = {v0, v1};
    return Exec(sql, values, sizeof(values) / sizeof(values[0]));
}


bool SqliteDatabase::Exec(const char *sql, const SqliteValue *values, int length) {
    SqliteResultSet result_set;
    SqliteResultSet *result_set_ptr = FindClosedCache(sql);
    if (!result_set_ptr) {
        result_set.stmt_ = StmtPrepare(sql);
        result_set_ptr = &result_set;
    }
    result_set_ptr->Open(this);

    if (result_set_ptr->stmt_ == nullptr) {
        return false;
    }

    BindValues(values, length, result_set_ptr->stmt_);
    result_set_ptr->Next();
    result_set_ptr->CloseNeedNoticeDb(false);
    return true;
}


bool SqliteDatabase::ExecBatch(const char *sql, const std::vector<SqliteValue *> &listValues, int length) {
    bool result = true;
    const int size = listValues.size();
    if (size <= 0 || length == 0) {
        return Exec(sql);
    }

    SqliteResultSet result_set;
    result_set.stmt_ = StmtPrepare("BEGIN EXCLUSIVE;");
    SqliteResultSet *result_set_ptr = &result_set;
    result_set_ptr->Open(this);

    if (result_set_ptr->stmt_ == nullptr) {
        return false;
    }

    result_set_ptr->Next();

    result_set.stmt_ = StmtPrepare(sql);
    for (int i = 0; i < size; i++) {
        SqliteValue *values = listValues[i];
        BindValues(values, length, result_set_ptr->stmt_);
        result &= result_set_ptr->Step();
        if (nullptr != values) {
            delete[] values;
            values = nullptr;
        }
    }

    result_set.stmt_ = StmtPrepare(result ? "COMMIT;" : "ROLLBACK;");
    result_set_ptr->Next();
    result_set_ptr->CloseNeedNoticeDb(false);

    return true;
}


SqliteResultSet *SqliteDatabase::Query(const char *sql) {
    return Query(sql, nullptr, 0);
}


SqliteResultSet *SqliteDatabase::Query(const char *sql, const SqliteValue &v0) {
    SqliteValue values[] = {v0};
    return Query(sql, values, sizeof(values) / sizeof(values[0]));
}


SqliteResultSet *SqliteDatabase::Query(const char *sql, const SqliteValue &v0, const SqliteValue &v1) {
    SqliteValue values[] = {v0, v1};
    return Query(sql, values, sizeof(values) / sizeof(values[0]));
}


SqliteResultSet *SqliteDatabase::Query(const char *sql,
                                       const SqliteValue *values,
                                       int length) {
    SqliteResultSet *result_set_ptr = FindClosedCache(sql);
    if (result_set_ptr) {
        open_result_sets_.push_back(result_set_ptr);
    } else if (reuse_result_set_.IsOpen()) {
        result_set_ptr = new SqliteResultSet();
        result_set_ptr->stmt_ = StmtPrepare(sql);
        open_result_sets_.push_back(result_set_ptr);
    } else {
        result_set_ptr = &reuse_result_set_;
        result_set_ptr->stmt_ = StmtPrepare(sql);
    }

    if (result_set_ptr->stmt_ == nullptr) {
        return nullptr;
    }

    BindValues(values, length, result_set_ptr->stmt_);
    result_set_ptr->Open(this);
    return result_set_ptr;
}

int SqliteDatabase::GetTableRows(const char *name) const {
    SqliteDatabase *db = const_cast<SqliteDatabase *>(this);
    char sql[256];
    sprintf(sql, "select count(*) from %s", name);
    SqliteResultSet *rs = db->Query(sql);

    if (rs) {
        int count = 0;
        if (rs->Next()) {
            count = rs->ColumnInt(0);
        }
        rs->Close();
        return count;
    }

    return 0;
}

bool SqliteDatabase::IsTableExists(const char *name) const {
    SqliteDatabase *db = const_cast<SqliteDatabase *>(this);
    SqliteResultSet *rs = db->Query("select count(*) from sqlite_master where type='table' and name = ?",
                                    SqlText(name));

    if (rs) {
        rs->Next();
        int count = rs->ColumnInt(0);
        rs->Close();
        return (count == 1);
    }

    return 0;
}

// 获取数据库文件的版本
int SqliteDatabase::GetVersion() const {
    SqliteDatabase *db = const_cast<SqliteDatabase *>(this);
    SqliteResultSet *rs = db->Query("PRAGMA user_version;");

    int version = 0;
    if (rs) {
        rs->Next();
        version = rs->ColumnInt(0);
        rs->Close();

    }

    return version;
}

void SqliteDatabase::SetVersion(int version) const {
    SqliteDatabase *db = const_cast<SqliteDatabase *>(this);

    char sql[128];
    sprintf(sql, "PRAGMA user_version=%d;", version);
    db->Exec(sql);
}