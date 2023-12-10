//
// Created by lixiaoqing on 2021/9/17.
//

#include <iostream>

#include "gtest/gtest.h"

#include "json_builder.h"
#include "json_utils.h"
#include "json/json.h"

TEST(JsonTest, JsonBuilder) {
    JsonBuilder builder1;
    builder1.Add("key1", true);
    builder1.Add("key2", 100);
    builder1.Add("key3", 10000);
    builder1.Add("key4", "Be foolish.");
    auto obj = builder1.Get();

    JsonBuilder builder2;
    builder2.Add("obj", obj);

    std::cout << Json::FastWriter().write(builder1.Get()) << std::endl;
    std::cout << Json::FastWriter().write(builder2.Get()) << std::endl;

    const std::string json = R"({"key1":true,"key2":100,"key3":10000,"key4":true})";
    Json::Value value;
    json_util::String2JsonValue(value, json);
    if (value.empty()) {
        std::cout << "字符串转Json格式失败！" << std::endl;
        return;
    }
    std::cout << json_util::GetInt(value, "key2", -1) << std::endl;
}

TEST(JsonTest, JsonUtils_ReadJsonFile) {
    std::cout << __PRETTY_FUNCTION__ << std::endl;
    char dir[128]{};
    getcwd(dir, sizeof(dir));
    const std::string filepath = std::string(dir) + "/../test/test.json";
    Json::Value root;
    bool isOk = json_util::ReadJsonFile(filepath, root);
    if (!isOk) {
        std::cout << "读取Json文件失败: " << filepath << std::endl;
        return;
    }
    std::cout << "读取json文件成功！" << std::endl;

    // 再转为 string
    std::cout << json_util::JsonValue2String(root) << std::endl;
}

TEST(JsonTest, JsonUtils_String2JsonValue) {
    const std::string jsonStr = R"({"code":"J000000","description":"注册成功","resultMap":{},"success":true})";
    Json::Value obj;
    bool isOk = json_util::String2JsonValue(obj, jsonStr);
    if (isOk) {
        std::cout << "成功！" << std::endl;
    } else {
        std::cout << "失败！" << std::endl;
    }
}
