//
// Created by boss on 2023/11/19.
//

#include <gtest/gtest.h>

#include "../src/log_model.h"

#define TAG "LogModelTest"

using namespace log;

TEST(LogModelTest, WriteLogToFile) {
    const std::string dir = "/home/Tookit/Log/test/build/";
    const auto log_data1 = std::make_shared<LogData>(LogType::Normal, LogLevel::Info, TAG, "This is a test log.\n");
    EXPECT_TRUE(log_data1 != nullptr);

    LogModel model(dir);
    auto res = model.WriteLogToFile(log_data1);
    EXPECT_TRUE(res);
}