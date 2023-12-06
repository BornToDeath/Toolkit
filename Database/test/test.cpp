//
// Created by lixiaoqing on 2021/9/9.
//

#include <iostream>
#include <memory>

#include "db_master.h"
#include "sqlite_database.h"


void test01();

int main() {
    test01();
    return 0;
}

void test01() {
    auto db_master = std::make_shared<DbMaster>();
    if (!db_master) {
        std::cout << "创建 DBMaster 失败！" << std::endl;
        return;
    }

    const std::string db_path = "/home/Tookit/Database/test/test.db";
    auto isOK = db_master->Open(db_path);
    if (!isOK) {
        std::cout << "打开数据库失败！" << std::endl;
        return;
    }

    auto version = db_master->GetVersion();
    std::cout << "数据库版本: " << version << std::endl;
}