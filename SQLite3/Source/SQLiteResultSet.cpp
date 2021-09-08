//
// Created by lixiaoqing on 2021/9/8.
//

#include "SQLiteResultSet.h"
#include "SQLiteDatabase.h"


SQLiteResultSet::SQLiteResultSet() {
    _db = nullptr;
    _pStmt = nullptr;
    _cached = false;
}


void SQLiteResultSet::close() {
    _closeNeedNoticeDb(true);
}


void SQLiteResultSet::_closeNeedNoticeDb(bool flag) {
    if (_pStmt && _db) {
        SQLiteDatabase *db = _db;
        _db = nullptr;

        sqlite3_reset(_pStmt);
        if (!_cached) {
            sqlite3_finalize(_pStmt);
            _pStmt = nullptr;
        }
        if (flag) {
            db->resultSetDidfinalize(this);
        }
    }
}


bool SQLiteResultSet::next() {
    bool needRetry = true;
    int numberOfRetries = _db->numberOfRetries();
    int rc = SQLITE_OK;

    for (int idx = 0; needRetry && idx <= numberOfRetries; idx++) {
        needRetry = false;
        rc = sqlite3_step(_pStmt);
        if (rc == SQLITE_BUSY || rc == SQLITE_LOCKED) {
            needRetry = true;
            if (SQLITE_LOCKED == rc) {
                rc = sqlite3_reset(_pStmt);
            }
            _db->waitForRetry();
        }
    }

    return (rc == SQLITE_ROW);
}

// 用于执行有前面sqlite3_prepare创建的准备语句
bool SQLiteResultSet::step() {
    int rc = sqlite3_step(_pStmt);
    sqlite3_reset(_pStmt);

    return (rc == SQLITE_DONE);
}