//
// Created by lixiaoqing on 2021/5/19.
//


/******** 系统头文件 *********/
#include <iostream>
#include <sys/stat.h>
#include <chrono>
#include <cstdarg>
#include <cstring>
#include <sstream>
#include <iomanip>

/******** 自定义头文件 *********/
#include "LogTools.h"


unsigned long long LogTools::currentTimeMills() {
    std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()
    );
    return ms.count();
}

void LogTools::printLog(LogLevel level, const char *tag, const char *threadName, const char *format, ...) {

    // 日志内容
    char logText[LOG_TEXT_MAX_LEN] = {0};

    va_list arg_list;
    va_start(arg_list, format);
    vsnprintf(logText, LOG_TEXT_MAX_LEN, format, arg_list);
    va_end(arg_list);

    // 日志级别
    const char *logLevel = logLevelName[static_cast<int>(level)];

    // 当前时间
    std::string now = LogTools::getCurrentDateTime("%Y-%m-%d %H:%M:%S");

    // 输出到控制台
    printf("%s | %s | %s | %s | %s\n", now.c_str(), logLevel, tag, threadName, logText);

//    android_LogPriority priority = ANDROID_LOG_INFO;
//
//    switch (level) {
//        case LogLevel::Debug:
//            priority = ANDROID_LOG_DEBUG;
//            break;
//        case LogLevel::Info:
//            priority = ANDROID_LOG_INFO;
//            break;
//        case LogLevel::Warn:
//            priority = ANDROID_LOG_WARN;
//            break;
//        case LogLevel::Error:
//            priority = ANDROID_LOG_ERROR;
//            break;
//        default:
//            break;
//    }
//
//    __android_log_print(priority, tag, "%s | %s", threadName, logText);
}

std::string LogTools::getLogStrategyName(LogStrategy strategy) {
    switch (strategy) {
        case LogStrategy::LOG_MMAP:
            return "LOG_MMAP";
        case LogStrategy::LOG_MEMORY:
            return "LOG_MEMORY";
        case LogStrategy::LOG_MMAP_FAIL:
            return "LOG_MMAP_FAIL";
        case LogStrategy::LOG_MEMORY_FAIL:
            return "LOG_MEMORY_FAIL";
        default:
            return "LOG_NO_STRATEGY";
    }
}

std::string LogTools::getLogTypeName(LogType type) {
    switch (type) {
        case LogType::Normal:
            return "normal";
        case LogType::Error:
            return "error";
        default:
            return "unknown";
    }
}

bool LogTools::isFileExist(const char *const &filePath) {
    return 0 == access(filePath, F_OK);
}

long LogTools::getFileSize(const char *const &filePath) {

    FILE *fp = fopen(filePath, "rb");
    if (fp == nullptr) {
        return -1;
    }

    fseek(fp, 0, SEEK_END);
    long fileSize = ftell(fp);
    fclose(fp);

    return fileSize;
}

bool LogTools::createDirIfNotExist(const char *const &dir) {
    if (dir == nullptr) {
        return false;
    }

    // 如果日志目录不存在则创建
    if (0 != access(dir, 0)) {
        if (0 != mkdir(dir, 0755)) {
            return false;
        }
    }
    return true;
}

bool LogTools::createMultiLevelDir(const char *dir) {
    if (dir == nullptr) {
        return false;
    }

    std::string root = dir;
    if (root[root.length() - 1] != '/') {
        root.push_back('/');
    }

    // 先尝试创建
    if (createDirIfNotExist(dir)) {
        return true;
    }

    // 再递归创建
    bool isOk = false;
    unsigned long offset = 1;

    do {
        auto index = root.find('/', offset);
        if (index == std::string::npos) {
            break;
        }
        std::string temp = root.substr(0, index);
        isOk = createDirIfNotExist(temp.c_str());
        offset = index + 1;
    } while (isOk);

    return isOk;
}

std::string LogTools::getCurrentDateTime(const char *const format) {

    {
        /**
         * 方式一
         * 示例：2021-08-04 19:56:03
         */

//        const int size = 64;
//        char now[size];
//
//        time_t curTimestamp;
//
//        // 获取时间戳
//        time(&curTimestamp);
//
//        // 转换为tm结构
//        tm *timeinfo = localtime(&curTimestamp);
//
//        // 格式化为标准时间。示例：%Y-%m-%d %H:%M:%S
//        strftime(now, size, format, timeinfo);
//
//        std::string ret(now);
//        return ret;
    }


    {

        /**
         * 方式二：精确到毫秒
         * 示例：2021-08-04 21:51:53.443
         */

        auto now = std::chrono::system_clock::now();
        auto curTime = std::chrono::system_clock::to_time_t(now);

        // 日期
        const int size = 64;
        char buf[size] = {0};
        strftime(buf, size, format, std::localtime(&curTime));

        // 毫秒
        auto mills = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;

        std::ostringstream oss;
        oss << buf
            << '.'
            << std::setw(3) << std::setfill('0')  // 宽度为3，空白用0填充
            << mills.count();

        return oss.str();
    }
}