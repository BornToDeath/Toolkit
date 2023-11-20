//
// Created by lixiaoqing on 2021/5/19.
//

#include "log_tool.h"

/******** 系统头文件 *********/
#include <sys/prctl.h>
#include <sys/stat.h>

#include <chrono>
#include <cstdarg>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <sstream>

/** 定义日志输出到标准输出时的颜色 */
#define LOG_COLOR_RESET "\033[0m"
#define LOG_COLOR_RED "\033[31m"
#define LOG_COLOR_PURPLE "\033[35m"

namespace log {
namespace tool {

unsigned long long CurrentTimeMills() {
    std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()
    );
    return ms.count();
}

void PrintLog(LogLevel level, const char *tag, const char *format, ...) {

    // 获取线程名
    char threadName[THREAD_NAME_MAX_LEN] = {0};
    prctl(PR_GET_NAME, threadName);

    // 日志内容
    char logText[LOG_TEXT_MAX_LEN] = {0};

    va_list arg_list;
    va_start(arg_list, format);
    vsnprintf(logText, LOG_TEXT_MAX_LEN, format, arg_list);
    va_end(arg_list);

    // 日志级别
    const char *logLevel = logLevelName[static_cast<int>(level)];

    // 当前时间
    std::string now = GetCurrentDateTime("%Y-%m-%d %H:%M:%S");

    std::ostringstream oss;
    oss << now
        << " | " << logLevel
        << " | " << tag
        << " | " << threadName
        << " | " << logText;

    std::string log = oss.str();
    switch (level) {
        case LogLevel::Error:
            log = LOG_COLOR_RED + log + LOG_COLOR_RESET;
            break;
        case LogLevel::Warn:
            log = LOG_COLOR_PURPLE + log + LOG_COLOR_RESET;
            break;
        default:
            break;
    }

    // 输出到控制台
    printf("%s\n", log.c_str());
}

std::string GetLogStrategyName(LogStrategy strategy) {
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

std::string GetLogTypeName(LogType type) {
    switch (type) {
        case LogType::Normal:
            return "normal";
        case LogType::Error:
            return "error";
        default:
            return "unknown";
    }
}

bool IsFileExist(const char *const &filePath) {
    return 0 == access(filePath, F_OK);
}

long GetFileSize(const char *const &filePath) {

    FILE *fp = fopen(filePath, "rb");
    if (fp == nullptr) {
        return -1;
    }

    fseek(fp, 0, SEEK_END);
    long fileSize = ftell(fp);
    fclose(fp);

    return fileSize;
}

bool CreateDirIfNotExist(const char *const &dir) {
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

bool CreateMultiLevelDir(const char *dir) {
    if (dir == nullptr) {
        return false;
    }

    std::string root = dir;
    if (root[root.length() - 1] != '/') {
        root.push_back('/');
    }

    // 先尝试创建
    if (CreateDirIfNotExist(dir)) {
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
        isOk = CreateDirIfNotExist(temp.c_str());
        offset = index + 1;
    } while (isOk);

    return isOk;
}

std::string GetCurrentDateTime(const char *const format) {

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

}  // namespace tool
}  // namespace log