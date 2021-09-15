//
// Created by lixiaoqing on 2021/9/14.
//

#include "DBMaster.h"
#include "SQLiteDatabase.h"
#include "SQLExecutor.h"


DBMaster::~DBMaster() {
    if (this->db) {
        this->db->close();
        delete this->db;
        this->db = nullptr;
    }
    delete this->sqlExecutor;
    this->sqlExecutor = nullptr;
}

bool DBMaster::open(const std::string &dbFilePath) {
    if (this->isOpened) {
        return true;
    }

    // 创建 SQLite3 的数据库对象
    this->db = new SQLiteDatabase();

    // 打开数据库
    auto isOk = this->db->open(dbFilePath.c_str());
    if (!isOk) {
        return false;
    }

    this->isOpened = true;

    // 打开成功后，初始化 SQLExecutor
    this->sqlExecutor = new SQLExecutor(this->db);

    return true;
}

ISQLExecutor *DBMaster::getSQLExecutor() {
    return this->sqlExecutor;
}

int DBMaster::getVersion() const {
    return this->db->getVersion();
}


