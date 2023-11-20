//
// Created by lixiaoqing on 2021/8/8.
//

#include <gtest/gtest.h>
#include <thread>

#include "log.h"

#define TAG "LogTest"

class LogTest : public testing::Test {
protected:
    void SetUp() override {
        Log::Init(dir_.c_str());
    }

    void TearDown() override {
        Log::Stop();
    }

    const std::string dir_{"/home/Tookit/Log/test/build/"};
};

TEST_F(LogTest, Init) {
//    EXPECT_TRUE(Log::GetIsInit());
    Log::Debug(TAG, "HHHHHHHello world");
    Log::Info(TAG, "HHHHHHHello world");
    Log::Warn(TAG, "HHHHHHHello world");
    Log::Error(TAG, "HHHHHHHello world");
}

TEST(LogTest2, Init) {
//    EXPECT_FALSE(Log::GetIsInit());

    EXPECT_FALSE(Log::Init(nullptr));
//    EXPECT_FALSE(Log::GetIsInit());

    EXPECT_TRUE(Log::Init("/home/Tookit/Log/test/build/"));
//    EXPECT_TRUE(Log::GetIsInit());

    Log::Debug(TAG, "HHHHHHHello world");
    Log::Info(TAG, "HHHHHHHello world");
    Log::Warn(TAG, "HHHHHHHello world");
    Log::Error(TAG, "HHHHHHHello world");

    Log::Stop();
//    EXPECT_FALSE(Log::GetIsInit());
}
