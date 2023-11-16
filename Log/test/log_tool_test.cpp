//
// Created by boss on 2023/11/16.
//

#include <gtest/gtest.h>
#include <thread>

#include "log.h"
#include "../src/log_tool.h"

#define TAG "LogToolTest"

using namespace log;

TEST(LogToolTest, CurrentTimeMills) {
    auto t1 = tool::CurrentTimeMills();
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    auto t2 = tool::CurrentTimeMills();
    EXPECT_LT(t1, t2);
    EXPECT_GT(t2 - t1, 500);
}

TEST(LogToolTest, PrintLog) {
    std::string log = "Hello, %s";
    tool::PrintLog(LogLevel::Debug, TAG, log.c_str(), "world");
    tool::PrintLog(LogLevel::Info, TAG, log.c_str(), "world");
    tool::PrintLog(LogLevel::Warn, TAG, log.c_str(), "world");
    tool::PrintLog(LogLevel::Error, TAG, log.c_str(), "world");
}