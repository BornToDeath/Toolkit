//
// Created by lixiaoqing on 2021/9/17.
//

#include <iostream>
#include "JsonUtils.h"
#include "JsonBuilder.h"
#include "json/json.h"

namespace Test {

    void testJson() {
        JsonBuilder builder1;
        builder1.add("key1", true);
        builder1.add("key2", 100);
        builder1.add("key3", 10000);
        builder1.add("key4", "Be foolish.");
        auto obj = builder1.get();

        JsonBuilder builder2;
        builder2.add("obj", obj);

        std::cout << Json::FastWriter().write(builder1.get()) << std::endl;
        std::cout << Json::FastWriter().write(builder2.get()) << std::endl;

        const std::string json = R"({"key1":true,"key2":100,"key3":10000,"key4":true})";
        Json::Value value;
        JsonUtils::string2JsonValue(value, json);
        if (value.empty()) {
            std::cout << "字符串转Json格式失败！" << std::endl;
            return;
        }
        std::cout << JsonUtils::getInt(value, "key2", -1) << std::endl;
    }

    void testJson2() {
        std::cout << __PRETTY_FUNCTION__ << std::endl;
        const std::string filePath = "/Users/lixiaoqing/Desktop/Projects/Toolkit/Json/test/test.json";
        Json::Value root;
        bool isOk = JsonUtils::readJsonFile(filePath, root);
        if(!isOk) {
            std::cout << "读取Json文件失败" << std::endl;
            return;
        }
        std::cout << "读取json文件成功！" << std::endl;

        // 再转为 string
        std::cout << JsonUtils::jsonValue2String(root) << std::endl;
    }

    void testJson3() {
        const std::string jsonStr = R"({"code":"J000000","description":"注册成功","resultMap":{},"success":true})";
        Json::Value obj;
        bool isOk = JsonUtils::string2JsonValue(obj, jsonStr);
        if (isOk) {
            std::cout << "成功！" << std::endl;
        } else {
            std::cout << "失败！" << std::endl;
        }
    }
}

int main() {
    Test::testJson2();
    return 0;
}
