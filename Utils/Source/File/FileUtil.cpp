//
// Created by lixiaoqing on 2021/6/22.
//

#include <unistd.h>
#include <sys/stat.h>
#include <fstream>
#include <string>
#include "File/FileUtil.h"
#include "File/MD5FileUtil.h"


bool FileUtil::isFileExist(const char *const filePath) {
    return 0 == access(filePath, F_OK);
}

std::string FileUtil::getFileMD5(const char *filePath) {
    std::string md5;
    if (filePath == nullptr) {
        return md5;
    }

    auto fileMD5 = MD5FileUtil::getFileMD5(filePath, 32);
    if (fileMD5 == nullptr) {
        return md5;
    }

    md5 = std::string(fileMD5);
    return md5;
}

bool FileUtil::createDirIfNotExist(const char *const &dir) {
    // 如果日志目录不存在则创建
    if (0 != ::access(dir, 0)) {
        if (0 != ::mkdir(dir, 0755)) {
            return false;
        }
    }
    return true;
}

long FileUtil::getFileSize(const char *const &filePath) {
    FILE *fp = fopen(filePath, "rb");
    if (fp == nullptr) {
        return -1;
    }

    fseek(fp, 0, SEEK_END);
    long fileSize = ftell(fp);
    fclose(fp);

    return fileSize;
}

void FileUtil::getFileBytes(const char *filePath, BYTE **data, long &length) {
    if (filePath == nullptr) {
        return;
    }

    FILE *fp = fopen(filePath, "rb");
    if (fp == nullptr) {
        return;
    }

    // 文件长度
    length = getFileSize(filePath);
    *data = new BYTE[length]();

    // 写数据
    fread(*data, length, 1, fp);
    fclose(fp);
}

void FileUtil::writeBinary2File(const char *filePath, const BYTE *data, int length) {
    auto file = fopen(filePath, "wb+");
    if (file == nullptr) {
        return;
    }
    fwrite(data, length, 1, file);
    fclose(file);
}

bool FileUtil::writeText2File(const std::string &filePath, const std::string &text, bool isDelete) {
    std::ios_base::openmode mode = std::ios::out;
    if (!isDelete) {
        mode |= std::ios::app;
    }
    std::ofstream output(filePath, mode);
    if (!output.is_open()) {
        return false;
    }
    output << text;
    output.close();
    return true;
}

std::string FileUtil::getFirstLine(const std::string &filePath) {
    std::string line;
    if (filePath.empty() || !isFileExist(filePath.c_str())) {
        return line;
    }

    std::ifstream input(filePath, std::ios::in);
    if (!input.is_open()) {
        return line;
    }

    // 读取文件第一行内容
    getline(input, line);
    return line;
}