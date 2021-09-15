//
// Created by lixiaoqing on 2021/9/15.
//

#include <iostream>
#include <memory>
#include "DBMaster.h"
#include "SharedPreference.h"

void testSP();


int main() {
    testSP();
    return 0;
}

void testSP() {

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

    SharedPreference::init(dbMaster.get());
    auto sp = SharedPreference::getSharedPreference("lxq");
    if (sp == nullptr) {
        std::cout << "SP 为空！" << std::endl;
        return;
    }

    // set 测试
    std::cout << sp->setBool("key1", true) << std::endl;
    std::cout << sp->setInt("key2", 10) << std::endl;
    std::cout << sp->setFloat("key3", 3.14) << std::endl;
    std::cout << sp->setDouble("key4", 3.1415926) << std::endl;
    std::cout << sp->setLongLong("key5", 1000) << std::endl;
    std::cout << sp->setString("key6", "Hello World") << std::endl;
    std::cout << "==============================" << std::endl;

    // get 测试
    std::cout << sp->getBool("key1", false) << std::endl;
    std::cout << sp->getInt("key2", -1) << std::endl;
    std::cout << sp->getFloat("key3", -1.0) << std::endl;
    std::cout << sp->getDouble("key4", -2.0) << std::endl;
    std::cout << sp->getLongLong("key5", -999) << std::endl;
    std::cout << sp->getString("key6","WTF.") << std::endl;
}