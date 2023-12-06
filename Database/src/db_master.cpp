//
// Created by lixiaoqing on 2021/9/14.
//

#include "db_master.h"

#include "sqlite_database.h"
#include "sql_executor_impl.h"


DbMaster::~DbMaster() {
    if (this->db_) {
        this->db_->Close();
        delete this->db_;
        this->db_ = nullptr;
    }
    delete this->sql_executor_;
    this->sql_executor_ = nullptr;
}

bool DbMaster::Open(const std::string &db_file_path) {
    if (this->is_opened_) {
        return true;
    }

    // 创建 Sqlite3 的数据库对象
    this->db_ = new SqliteDatabase();

    // 打开数据库
    auto isOk = this->db_->Open(db_file_path.c_str());
    if (!isOk) {
        return false;
    }

    this->is_opened_ = true;

    // 打开成功后，初始化 SQLExecutor
    this->sql_executor_ = new SqlExecutorImpl(this->db_);

    return true;
}

SqlExecutor *DbMaster::GetSQLExecutor() {
    return this->sql_executor_;
}

int DbMaster::GetVersion() const {
    return this->db_->GetVersion();
}


