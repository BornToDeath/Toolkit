//
// Created by lixiaoqing on 2021/9/17.
//

#include <iostream>
#include "JsonUtils.h"
#include "JsonBuilder.h"
#include "json/json.h"

void testJson();


int main() {
    testJson();
    return 0;
}

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