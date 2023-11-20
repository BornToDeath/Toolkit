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
    EXPECT_GE(t2 - t1, 500);
}

TEST(LogToolTest, PrintLog) {
    std::string log = "Hello, %s";
    tool::PrintLog(LogLevel::Debug, TAG, log.c_str(), "world");
    tool::PrintLog(LogLevel::Info, TAG, log.c_str(), "world");
    tool::PrintLog(LogLevel::Warn, TAG, log.c_str(), "world");
    tool::PrintLog(LogLevel::Error, TAG, log.c_str(), "world");
}

TEST(LogToolTest, GetLogStrategyName) {
    auto name = tool::GetLogStrategyName(LogStrategy::LOG_MEMORY_FAIL);
    EXPECT_STREQ(name.c_str(), "LOG_MEMORY_FAIL");

    name = tool::GetLogStrategyName(LogStrategy::LOG_MMAP_FAIL);
    EXPECT_STREQ(name.c_str(), "LOG_MMAP_FAIL");

    name = tool::GetLogStrategyName(LogStrategy::LOG_MEMORY);
    EXPECT_STREQ(name.c_str(), "LOG_MEMORY");

    name = tool::GetLogStrategyName(LogStrategy::LOG_MMAP);
    EXPECT_STREQ(name.c_str(), "LOG_MMAP");

    name = tool::GetLogStrategyName(static_cast<LogStrategy>(10));
    EXPECT_STREQ(name.c_str(), "LOG_NO_STRATEGY");
}

TEST(LogToolTest, GetLogTypeName) {
    auto name = tool::GetLogTypeName(LogType::Normal);
    EXPECT_STREQ(name.c_str(), "normal");

    name = tool::GetLogTypeName(LogType::Error);
    EXPECT_STREQ(name.c_str(), "error");

    name = tool::GetLogTypeName(LogType(5));
    EXPECT_STREQ(name.c_str(), "unknown");
}

TEST(LogToolTest, IsFileExist) {
    const std::string dir = "/home/Tookit/Log/test/";
    const std::string filepath1 = dir + "unit_test.sh";
    auto res = tool::IsFileExist(filepath1.c_str());
    EXPECT_TRUE(res);

    const std::string filepath2 = dir + "aaa";
    ::remove(filepath2.c_str());
    res = tool::IsFileExist(filepath2.c_str());
    EXPECT_FALSE(res);
}

TEST(LogToolTest, GetFileSize) {
    const std::string dir = "/home/Tookit/Log/test/";
    const std::string filepath1 = dir + "unit_test.sh";
    auto size = tool::GetFileSize(filepath1.c_str());
    EXPECT_GT(size, 0);
    EXPECT_NE(size, -1);

    const std::string filepath2 = dir + "aaa";
    ::remove(filepath2.c_str());
    size = tool::GetFileSize(filepath2.c_str());
    EXPECT_EQ(size, -1);
}

TEST(LogToolTest, CreateDirIfNotExist) {
    const std::string root_dir = "/home/Tookit/Log/test/build/";
    auto res = tool::CreateDirIfNotExist(nullptr);
    EXPECT_FALSE(res);

    const std::string dir1 = root_dir + "build";
    res = tool::CreateDirIfNotExist(dir1.c_str());
    EXPECT_TRUE(res);

    const std::string dir2 = root_dir + "aaa";
    ::remove(dir2.c_str());
    res = tool::CreateDirIfNotExist(dir2.c_str());
    EXPECT_TRUE(res);
}

TEST(LogToolTest, CreateMultiLevelDir) {
    const std::string root_dir = "/home/Tookit/Log/test/build/";
    auto res = tool::CreateMultiLevelDir(nullptr);
    EXPECT_FALSE(res);

    const std::string dir1 = root_dir + "a/b/c/d";
    res = tool::CreateMultiLevelDir(dir1.c_str());
    EXPECT_TRUE(res);

    res = tool::CreateMultiLevelDir(dir1.c_str());
    EXPECT_TRUE(res);
}

TEST(LogToolTest, GetCurrentDateTime) {
    std::string demo = "2023-11-11 11:11:11.111";
    auto date = tool::GetCurrentDateTime("%Y-%m-%d %H:%M:%S");
    EXPECT_EQ(demo.length(), date.length());
}