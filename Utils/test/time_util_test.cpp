//
// Created by boss on 2023/11/27.
//

#include "gtest/gtest.h"
#include "time/time_util.h"

using namespace utils::time_util;

TEST(TimeUtilTest, TimeUtil) {
    auto time_ms = GetCurrentTimeMills();
    EXPECT_TRUE(std::to_string(time_ms).length() == 13);

    auto date = GetCurrentDateTime();
    EXPECT_FALSE(date.empty());
}