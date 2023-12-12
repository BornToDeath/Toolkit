//
// Created by lixiaoqing on 2021/9/15.
//

#include <iostream>
#include <memory>

#include <unistd.h>

#include "db_master.h"
#include "shared_preference.h"

void testSP();


int main() {
    testSP();
    return 0;
}

void testSP() {

    auto dbMaster = std::make_shared<DbMaster>();
    if (!dbMaster) {
        std::cout << "创建 DbMaster 失败！" << std::endl;
        return;
    }

    char dir[128]{};
    getcwd(dir, sizeof(dir));
    const std::string dbPath = std::string(dir) + "/test.db";
    auto isOK = dbMaster->Open(dbPath);
    if (!isOK) {
        std::cout << "打开数据库失败！" << std::endl;
        return;
    }

    auto version = dbMaster->GetVersion();
    std::cout << "数据库版本: " << version << std::endl;

    SharedPreference::Init(dbMaster.get());
    auto sp = SharedPreference::GetSharedPreference("lxq");
    if (sp == nullptr) {
        std::cout << "SP 为空！" << std::endl;
        return;
    }

    // set 测试
    std::cout << sp->SetBool("key1", true) << std::endl;
    std::cout << sp->SetInt("key2", 10) << std::endl;
    std::cout << sp->SetFloat("key3", 3.14) << std::endl;
    std::cout << sp->SetDouble("key4", 3.1415926) << std::endl;
    std::cout << sp->SetLongLong("key5", 1000) << std::endl;
    std::cout << sp->SetString("key6", "Hello World") << std::endl;
    std::cout << "==============================" << std::endl;

    // get 测试
    std::cout << sp->GetBool("key1", false) << std::endl;
    std::cout << sp->GetInt("key2", -1) << std::endl;
    std::cout << sp->GetFloat("key3", -1.0) << std::endl;
    std::cout << sp->GetDouble("key4", -2.0) << std::endl;
    std::cout << sp->GetLongLong("key5", -999) << std::endl;
    std::cout << sp->GetString("key6", "WTF.") << std::endl;
}