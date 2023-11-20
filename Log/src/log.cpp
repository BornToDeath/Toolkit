//
// Created by lixiaoqing on 2021/8/8.
//

#include "log.h"

/**
 * 系统头文件
 */
#include <atomic>
#include <cstdarg>
#include <cstring>
#include <queue>
#include <thread>

/**
 * 自定义头文件
 */
#include "logger.h"
#include "log_tool.h"

/**
 * 宏定义
 */
#define TAG "Log"

using namespace log;

// 标识日志模块是否进行了初始化
static std::atomic<bool> is_init(false);

bool Log::Init(const char *const root_dir) {

    if (DEBUG) {
        tool::PrintLog(LogLevel::Debug, TAG, ">>> 初始化日志模块");
    }

    if (root_dir == nullptr || strlen(root_dir) == 0 || !tool::CreateMultiLevelDir(root_dir)) {
        if (DEBUG) {
            tool::PrintLog(LogLevel::Error, TAG,
                           ">>> 日志根目录初始化失败! 日志根目录: %s, errno=%d (%s)",
                           root_dir, errno, strerror(errno));
        }
        return false;
    }

    std::string root_dir_str(root_dir);
    if (root_dir_str[root_dir_str.length() - 1] != '/') {
        root_dir_str.push_back('/');
    }

    if (!Logger::Singleton().Init(root_dir_str)) {
        tool::PrintLog(LogLevel::Error, TAG, ">>> Logger Init failed");
        return false;
    }

    if (DEBUG) {
        tool::PrintLog(LogLevel::Debug, TAG, ">>> 日志存储根目录为: %s", root_dir_str.c_str());
    }

    // 设置初始化标志位为 true
    is_init.store(true);
    return true;
}

void Log::Stop() {
    is_init.store(false);
    Logger::Singleton().Stop();
}

void Log::Debug(const char *tag, const char *format, ...) {

    // 对可变参数进行组合，合成一条完整的日志数据
    char log_text[LOG_TEXT_MAX_LEN] = {0};
    va_list arg_list;
    va_start(arg_list, format);
    vsnprintf(log_text, LOG_TEXT_MAX_LEN, format, arg_list);
    va_end(arg_list);

    Log::Save(LogType::Normal, LogLevel::Debug, tag, log_text);
}

void Log::Info(const char *tag, const char *format, ...) {

    // 对可变参数进行组合，合成一条完整的日志数据
    char log_text[LOG_TEXT_MAX_LEN] = {0};
    va_list arg_list;
    va_start(arg_list, format);
    vsnprintf(log_text, LOG_TEXT_MAX_LEN, format, arg_list);
    va_end(arg_list);

    Log::Save(LogType::Normal, LogLevel::Info, tag, log_text);
}

void Log::Warn(const char *tag, const char *format, ...) {

    // 对可变参数进行组合，合成一条完整的日志数据
    char log_text[LOG_TEXT_MAX_LEN] = {0};
    va_list arg_list;
    va_start(arg_list, format);
    vsnprintf(log_text, LOG_TEXT_MAX_LEN, format, arg_list);
    va_end(arg_list);

    Log::Save(LogType::Normal, LogLevel::Warn, tag, log_text);
}

void Log::Error(const char *tag, const char *format, ...) {

    // 对可变参数进行组合，合成一条完整的日志数据
    char log_text[LOG_TEXT_MAX_LEN] = {0};
    va_list arg_list;
    va_start(arg_list, format);
    vsnprintf(log_text, LOG_TEXT_MAX_LEN, format, arg_list);
    va_end(arg_list);

    Log::Save(LogType::Error, LogLevel::Error, tag, log_text);
}

bool Log::Save(LogType type, LogLevel level, const char *tag, const char *log) {

    // 日志输出到控制台
    tool::PrintLog(level, tag, "%s", log);

    if (!is_init) {
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