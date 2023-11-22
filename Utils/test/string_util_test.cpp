//
// Created by boss on 2023/11/22.
//

#include <numeric>

#include "gtest/gtest.h"
#include "string/string_util.h"

#define LOG(format, ...) printf(format "\n", ##__VA_ARGS__)

using namespace utils::string_util;

TEST(StringUtilTest, Compare) {
    EXPECT_TRUE(Compare(std::string("aaaa"), std::string("aaaa")));
    EXPECT_FALSE(Compare(std::string("aaaa"), std::string("aaab")));
    EXPECT_FALSE(Compare(std::string("aaaa "), std::string("aaaa")));
    EXPECT_FALSE(Compare(std::string("aaaa"), std::string(" aaaa")));
    EXPECT_FALSE(Compare(std::string("aaaa"), std::string("")));

    EXPECT_TRUE(Compare("aaaa", "aaaa"));
    EXPECT_FALSE(Compare("aaaa", "aaab"));
    EXPECT_FALSE(Compare("aaaa ", "aaaa"));
    EXPECT_FALSE(Compare("aaaa", " aaaa"));
    EXPECT_FALSE(Compare("aaaa", ""));
}

TEST(StringUtilTest, String2Int) {
    EXPECT_EQ(String2Int("123"), 123);
    EXPECT_EQ(String2Int("123a"), 123);
    EXPECT_EQ(String2Int("a123"), 0);
}

TEST(StringUtilTest, Trim) {
    std::string str1 = "    sdasdasd";
    Trim(str1);
    EXPECT_STREQ(str1.c_str(), "sdasdasd");

    std::string str2 = "    sdasdasd     ";
    Trim(str2);
    EXPECT_STREQ(str2.c_str(), "sdasdasd");

    std::string str3 = "             ";
    Trim(str3);
    EXPECT_TRUE(str3.empty());

    std::string str4 = "";
    Trim(str4);
    EXPECT_TRUE(str4.empty());
}

TEST(StringUtilTest, GetMd5) {
    EXPECT_TRUE(GetMd5("").empty());
    EXPECT_STREQ(GetMd5("Hello world").c_str(), "3E25960A79DBC69B674CD4EC67A72C62");
    EXPECT_STREQ(GetMd5("Hello world.").c_str(), "764569E58F53EA8B6404F6FA7FC0247F");
}

TEST(StringUtilTest, Split) {
    EXPECT_TRUE(Split("", ' ').empty());
    const std::string s = "This is a text log";
    auto words = Split(s, ' ');
//    for (const auto &w: words) {
//        LOG("%s", w.c_str());
//    }
    ASSERT_EQ(words.size(), 5);
    EXPECT_STREQ(words[0].c_str(), "This");
    EXPECT_STREQ(words[1].c_str(), "is");
    EXPECT_STREQ(words[2].c_str(), "a");
    EXPECT_STREQ(words[3].c_str(), "text");
    EXPECT_STREQ(words[4].c_str(), "log");
    auto combined_string = std::accumulate(
            std::next(words.begin()), words.end(), words.front(),
            [](const std::string &s1, const std::string &s2) {
                return s1 + ' ' + s2;
            });
    EXPECT_STREQ(s.c_str(), combined_string.c_str());
}

