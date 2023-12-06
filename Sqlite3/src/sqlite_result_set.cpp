//
// Created by lixiaoqing on 2021/9/8.
//

#include "sqlite_result_set.h"

#include "sqlite_database.h"


SqliteResultSet::SqliteResultSet() {
    db_ = nullptr;
    stmt_ = nullptr;
    cached_ = false;
}


void SqliteResultSet::Close() {
    CloseNeedNoticeDb(true);
}


void SqliteResultSet::CloseNeedNoticeDb(bool flag) {
    if (stmt_ && db_) {
        SqliteDatabase *db = db_;
        db_ = nullptr;

        sqlite3_reset(stmt_);
        if (!cached_) {
            sqlite3_finalize(stmt_);
            stmt_ = nullptr;
        }
        if (flag) {
            db->ResultSetDidfinalize(this);
        }
    }
}


bool SqliteResultSet::Next() {
    bool need_retry = true;
    int number_of_retries = db_->NumberOfRetries();
    int rc = SQLITE_OK;

    for (int idx = 0; need_retry && idx <= number_of_retries; idx++) {
        need_retry = false;
        rc = sqlite3_step(stmt_);
        if (rc == SQLITE_BUSY || rc == SQLITE_LOCKED) {
            need_retry = true;
            if (SQLITE_LOCKED == rc) {
                rc = sqlite3_reset(stmt_);
            }
            db_->WaitForRetry();
        }
    }

    return (rc == SQLITE_ROW);
}

// 用于执行有前面sqlite3_prepare创建的准备语句
bool SqliteResultSet::Step() {
    int rc = sqlite3_step(stmt_);
    sqlite3_reset(stmt_);

    return (rc == SQLITE_DONE);
}