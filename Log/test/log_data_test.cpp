//
// Created by boss on 2023/11/20.
//

#include <gtest/gtest.h>
#include "../src/log_data.h"

#define TAG "LogDataTest"

using namespace log;

TEST(LogDataTest, GetLog) {
    const auto log_data = std::make_shared<LogData>(
            LogType::Normal,
            LogLevel::Info,
            TAG,
            "Hello world");
    EXPECT_EQ(log_data->GetLevel(), LogLevel::Info);
    EXPECT_EQ(log_data->GetType(), LogType::Normal);
    EXPECT_STREQ(log_data->GetTag().c_str(), TAG);
    EXPECT_STREQ(log_data->GetLog().c_str(), "Hello world");
    log_data->SetLog("This is a log");
    EXPECT_STREQ(log_data->GetLog().c_str(), "This is a log");
}