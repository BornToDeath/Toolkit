//
// Created by lixiaoqing on 2021/7/15.
//

#include <unistd.h>

#include <gtest/gtest.h>
#include <iostream>

#include "common/common_util.h"

using namespace utils::common_util;

TEST(CommonUtilTest, Bytes2Int) {
    EXPECT_EQ(Bytes2Int(nullptr), 0);
    EXPECT_EQ(Bytes2Int(nullptr, -1), 0);

    const int num = 10;
    unsigned char buf[8]{};
    memcpy(buf, &num, sizeof(num));
    EXPECT_EQ(Bytes2Int(buf, -1), 0);
    EXPECT_EQ(Bytes2Int(buf, 0), 10);
}

TEST(CommonUtilTest, GetEnumValueOfInt) {
    enum class Animal {
        CAT = 0,
        COW,
        DOG,
        DUCK,
        PIG,
        SHEEP
    };
    EXPECT_EQ(GetEnumValueOfInt<Animal>(0), Animal::CAT);
    EXPECT_EQ(GetEnumValueOfInt<Animal>(2), Animal::DOG);
    EXPECT_NE(GetEnumValueOfInt<Animal>(6), Animal::CAT);
    EXPECT_NE(GetEnumValueOfInt<Animal>(6), Animal::SHEEP);
}

TEST(CommonUtilTest, GetIntValueOfEnum) {
    enum class Animal {
        CAT = 0,
        COW,
        DOG,
        DUCK,
        PIG,
        SHEEP
    };
    EXPECT_EQ(GetIntValueOfEnum<Animal>(Animal::CAT), 0);
    EXPECT_EQ(GetIntValueOfEnum<Animal>(Animal::DOG), 2);
    EXPECT_EQ(GetIntValueOfEnum<Animal>(Animal::SHEEP), 5);
}

TEST(CommonUtilTest, Print) {
    Print("Hello, world");
    Print("This is a %s", "test log");
}