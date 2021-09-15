//
// Created by lixiaoqing on 2021/9/9.
//

#include <iostream>
#include <memory>
#include "SQLiteDatabase.h"
#include "DBMaster.h"


void test01();

int main() {
    test01();
    return 0;
}

void test01() {
    auto dbMaster = std::make_shared<DBMaster>();
    if (!dbMaster) {
        std::cout << "创建 DBMaster 失败！" << std::endl;
        return;
    }

    const std::string dbPath = "/home/nvidia/Toolkit/Database/test/test.db";
    auto isOK = dbMaster->open(dbPath);
    if (!isOK) {
        std::cout << "打开数据库失败！" << std::endl;
        return;
    }

    auto version = dbMaster->getVersion();
    std::cout << "数据库版本: " << version << std::endl;
}