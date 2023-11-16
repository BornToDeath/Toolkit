//
// Created by lixiaoqing on 2021/8/8.
//

/**
 * 系统头文件
 */
#include <thread>
#include <queue>
#include <cstring>
#include <cstdarg>
#include <atomic>

/**
 * 自定义头文件
 */
#include "log.h"
#include "logger.h"
#include "log_tool.h"

/**
 * 宏定义
 */
#define TAG "Log"

using namespace log;

// 标识日志模块是否进行了初始化
static std::atomic<bool> isInit(false);

bool Log::Init(const char *const rootDir) {

    if (DEBUG) {
        tool::PrintLog(LogLevel::Debug, TAG, ">>> 初始化日志模块");
    }

    if (rootDir == nullptr || strlen(rootDir) == 0 || !tool::CreateMultiLevelDir(rootDir)) {
        if (DEBUG) {
            tool::PrintLog(LogLevel::Error, TAG,
                           ">>> 日志根目录初始化失败! 日志根目录: %s, errno=%d (%s)",
                           rootDir, errno, strerror(errno));
        }
        return false;
    }

    std::string rootDirStr(rootDir);
    if (rootDirStr[rootDirStr.length() - 1] != '/') {
        rootDirStr.push_back('/');
    }

    if (!Logger::Singleton().Init(rootDirStr)) {
        tool::PrintLog(LogLevel::Error, TAG, ">>> Logger Init failed");
        return false;
    }

    if (DEBUG) {
        tool::PrintLog(LogLevel::Debug, TAG, ">>> 日志存储根目录为: %s", rootDirStr.c_str());
    }

    // 设置初始化标志位为 true
    isInit.store(true);
    return true;
}

void Log::Stop() {
    isInit.store(false);
    Logger::Singleton().Stop();
}

void Log::Debug(const char *tag, const char *format, ...) {

    // 对可变参数进行组合，合成一条完整的日志数据
    char logText[LOG_TEXT_MAX_LEN] = {0};
    va_list arg_list;
    va_start(arg_list, format);
    vsnprintf(logText, LOG_TEXT_MAX_LEN, format, arg_list);
    va_end(arg_list);

    Log::Save(LogType::Normal, LogLevel::Debug, tag, logText);
}

void Log::Info(const char *tag, const char *format, ...) {

    // 对可变参数进行组合，合成一条完整的日志数据
    char logText[LOG_TEXT_MAX_LEN] = {0};
    va_list arg_list;
    va_start(arg_list, format);
    vsnprintf(logText, LOG_TEXT_MAX_LEN, format, arg_list);
    va_end(arg_list);

    Log::Save(LogType::Normal, LogLevel::Info, tag, logText);
}

void Log::Warn(const char *tag, const char *format, ...) {

    // 对可变参数进行组合，合成一条完整的日志数据
    char logText[LOG_TEXT_MAX_LEN] = {0};
    va_list arg_list;
    va_start(arg_list, format);
    vsnprintf(logText, LOG_TEXT_MAX_LEN, format, arg_list);
    va_end(arg_list);

    Log::Save(LogType::Normal, LogLevel::Warn, tag, logText);
}

void Log::Error(const char *tag, const char *format, ...) {

    // 对可变参数进行组合，合成一条完整的日志数据
    char logText[LOG_TEXT_MAX_LEN] = {0};
    va_list arg_list;
    va_start(arg_list, format);
    vsnprintf(logText, LOG_TEXT_MAX_LEN, format, arg_list);
    va_end(arg_list);

    Log::Save(LogType::Error, LogLevel::Error, tag, logText);
}

bool Log::Save(LogType type, LogLevel level, const char *tag, const char *log) {

    // 日志输出到控制台
    tool::PrintLog(level, tag, "%s", log);

    if (!isInit) {
        tool::PrintLog(level, tag, ">>> 日志根目录尚未初始化, 日志存储失败!");
        return false;
    }

    // 存储日志
    Logger::Singleton().WriteLog(type, level, tag, log);
    if (type == LogType::Error) {
        // Normal 文件中也记录一份 Error 日志, 方便阅读
        Logger::Singleton().WriteLog(LogType::Normal, level, tag, log);
    }
    return true;
}