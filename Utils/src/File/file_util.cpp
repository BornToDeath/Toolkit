//
// Created by lixiaoqing on 2021/6/22.
//

#include "file/file_util.h"

#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>

#include <fstream>
#include <string>

#include "md5_file_util.h"

namespace utils {
namespace file_util {

bool IsFileExist(const char *const filepath) {
    return 0 == access(filepath, F_OK);
}

bool IsFolderExist(const char *const folder_path) {
    return 0 == access(folder_path, F_OK);
}

std::string GetFileMD5(const char *filepath) {
    std::string md5;
    if (filepath == nullptr) {
        return md5;
    }

    auto file_md5 = md5_file_util::GetFileMD5(filepath, 32);
    if (file_md5 == nullptr) {
        return md5;
    }

    md5 = std::string(file_md5);
    return md5;
}

bool CreateDirIfNotExist(const char *const &dir) {
    // 如果日志目录不存在则创建
    if (0 != ::access(dir, 0)) {
        if (0 != ::mkdir(dir, 0755)) {
            return false;
        }
    }
    return true;
}

long GetFileSize(const char *const &filepath) {
    FILE *fp = fopen(filepath, "rb");
    if (fp == nullptr) {
        return -1;
    }

    fseek(fp, 0, SEEK_END);
    long fileSize = ftell(fp);
    fclose(fp);

    return fileSize;
}

void GetFileBytes(const char *filepath, BYTE **data, long &length) {
    if (filepath == nullptr) {
        return;
    }

    FILE *fp = fopen(filepath, "rb");
    if (fp == nullptr) {
        return;
    }

    // 文件长度
    length = GetFileSize(filepath);
    *data = new BYTE[length]();

    // 写数据
    fread(*data, length, 1, fp);
    fclose(fp);
}

void WriteBinaryToFile(const char *filepath, const BYTE *data, int length) {
    auto file = fopen(filepath, "wb+");
    if (file == nullptr) {
        return;
    }
    fwrite(data, length, 1, file);
    fclose(file);
}

bool WriteTextToFile(const std::string &filepath, const std::string &text, bool isDelete) {
    std::ios_base::openmode mode = std::ios::out;
    if (!isDelete) {
        mode |= std::ios::app;
    }
    std::ofstream output(filepath, mode);
    if (!output.is_open()) {
        return false;
    }
    output << text;
    output.close();
    return true;
}

std::string GetFirstLine(const std::string &filepath) {
    std::string line;
    if (filepath.empty() || !IsFileExist(filepath.c_str())) {
        return line;
    }

    std::ifstream input(filepath, std::ios::in);
    if (!input.is_open()) {
        return line;
    }

    // 读取文件第一行内容
    getline(input, line);
    return line;
}


void DeleteAllFiles(const std::string &folder) {
    if (folder.empty() || !IsFolderExist(folder.c_str())) {
        return;
    }

    DIR *dir = opendir(folder.c_str());
    if (dir == nullptr) {
        return;
    }

    // 遍历目录
    struct dirent *ent;
    while ((ent = readdir(dir)) != nullptr) {
        std::string filePath = folder + ent->d_name;
        ::remove(filePath.c_str());
    }
    closedir(dir);
}

}  // namespace file_util
}  // namespace utils