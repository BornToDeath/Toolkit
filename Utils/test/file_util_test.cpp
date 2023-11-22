//
// Created by boss on 2023/11/22.
//

#include "common/common_util.h"
#include "file/file_util.h"
#include "gtest/gtest.h"

using namespace utils::file_util;
using namespace utils::common_util;

TEST(FileUtilTest, FileUtilTest) {
    char dir[512];
    getcwd(dir, sizeof(dir));
    Print("pwd: %s", dir);

    const std::string not_exist_file_path = std::string(dir) + "/aaa";
    EXPECT_FALSE(IsFileExist(not_exist_file_path.c_str()));
    EXPECT_FALSE(IsFolderExist(not_exist_file_path.c_str()));
    EXPECT_TRUE(GetFileMD5(not_exist_file_path.c_str()).empty());

    const std::string exist_file_path = std::string(dir) + "/../unit_test.sh";
    EXPECT_TRUE(IsFileExist(exist_file_path.c_str()));
    const auto md5 = GetFileMD5(exist_file_path.c_str());
    EXPECT_EQ(md5.length(), 32);
    EXPECT_STREQ(md5.c_str(), "a56ee9988f9dc618c81a99a6ee881344");
    EXPECT_GT(GetFileSize(exist_file_path.c_str()), 0);

    const std::string folder_path = std::string(dir) + "/CMakeFiles/";
    EXPECT_TRUE(IsFolderExist(folder_path.c_str()));

    const std::string not_exist_folder_path = folder_path + "/aaa/";
    ::remove(not_exist_folder_path.c_str());
    EXPECT_TRUE(CreateDirIfNotExist(not_exist_folder_path.c_str()));

    const std::string temp_folder_path = not_exist_folder_path + "/bbb/";
    CreateDirIfNotExist(temp_folder_path.c_str());
    EXPECT_TRUE(IsFolderExist(temp_folder_path.c_str()));
    DeleteAllFiles(not_exist_folder_path);
    EXPECT_FALSE(IsFolderExist(temp_folder_path.c_str()));

    std::string text = "Hello world";
    BYTE *buf = const_cast<BYTE *>(reinterpret_cast<const BYTE *>(text.c_str()));
    const std::string write_file_path = std::string(dir) + "/test.log";
    WriteBinaryToFile(write_file_path.c_str(), buf, text.length());
    EXPECT_TRUE(IsFileExist(write_file_path.c_str()));

    BYTE *text_buf = nullptr;
    long length = 0;
    GetFileBytes(write_file_path.c_str(), &text_buf, length);
    Print("====================\n%s\n====================", text_buf);
    EXPECT_TRUE(text_buf != nullptr);

    EXPECT_TRUE(WriteTextToFile(write_file_path, text, true));
    auto line = GetFirstLine(write_file_path);
    EXPECT_STREQ(line.c_str(), text.c_str());

    ::remove(write_file_path.c_str());
    EXPECT_FALSE(IsFileExist(write_file_path.c_str()));
}