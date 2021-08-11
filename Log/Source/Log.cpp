//
// Created by lixiaoqing on 2021/8/8.
//

/**
 * 系统头文件
 */
#include <thread>
#include <sys/prctl.h>
#include <queue>
#include <cstring>
#include <cstdarg>
#include <atomic>

/**
 * 自定义头文件
 */
#include "Log.h"
#include "LogImpl.h"
#include "LogTools.h"

/**
 * 宏定义
 */
#define TAG "Log"


// 标识日志模块是否进行了初始化
static std::atomic<bool> isInit(false);


bool Log::init(const char *const rootDir) {

    // 获取线程名
    char threadName[THREAD_NAME_MAX_LEN] = {0};
    prctl(PR_GET_NAME, threadName);

    if (DEBUG) {
        LogTools::printLog(LogLevel::Debug, TAG, threadName, ">>> 初始化日志模块");
    }

    if (rootDir == nullptr || strlen(rootDir) == 0 || !LogTools::createMultiLevelDir(rootDir)) {
        if (DEBUG) {
            LogTools::printLog(LogLevel::Error, TAG, threadName,
                               ">>> 日志根目录初始化失败！日志根目录：%s，errno = %d", rootDir, errno);
        }
        return false;
    }

    // 设置初始化标志位为 true
    isInit.store(true);

    std::string rootDirStr(rootDir);
    if (rootDirStr[rootDirStr.length() - 1] != '/') {
        rootDirStr.push_back('/');
    }

    // 将 logRootDir 传递给 LogImpl
    LogImpl::setLogRootDir(rootDirStr);

    if (DEBUG) {
        LogTools::printLog(LogLevel::Debug, TAG, threadName, ">>> 日志存储根目录为：%s", rootDirStr.c_str());
    }

    return true;
}

void Log::release() {
    isInit.store(false);
    LogImpl::getInstance()->releaseSingleton();
}

void Log::debug(const char *tag, const char *format, ...) {

    // 对可变参数进行组合，合成一条完整的日志数据
    char logText[LOG_TEXT_MAX_LEN] = {0};
    va_list arg_list;
    va_start(arg_list, format);
    vsnprintf(logText, LOG_TEXT_MAX_LEN, format, arg_list);
    va_end(arg_list);

    Log::save(LogType::Normal, LogLevel::Debug, tag, logText);
}

void Log::info(const char *tag, const char *format, ...) {

    // 对可变参数进行组合，合成一条完整的日志数据
    char logText[LOG_TEXT_MAX_LEN] = {0};
    va_list arg_list;
    va_start(arg_list, format);
    vsnprintf(logText, LOG_TEXT_MAX_LEN, format, arg_list);
    va_end(arg_list);

    Log::save(LogType::Normal, LogLevel::Info, tag, logText);
}

void Log::warn(const char *tag, const char *format, ...) {

    // 对可变参数进行组合，合成一条完整的日志数据
    char logText[LOG_TEXT_MAX_LEN] = {0};
    va_list arg_list;
    va_start(arg_list, format);
    vsnprintf(logText, LOG_TEXT_MAX_LEN, format, arg_list);
    va_end(arg_list);

    Log::save(LogType::Normal, LogLevel::Warn, tag, logText);
}

void Log::error(const char *tag, const char *format, ...) {

    // 对可变参数进行组合，合成一条完整的日志数据
    char logText[LOG_TEXT_MAX_LEN] = {0};
    va_list arg_list;
    va_start(arg_list, format);
    vsnprintf(logText, LOG_TEXT_MAX_LEN, format, arg_list);
    va_end(arg_list);

    Log::save(LogType::Error, LogLevel::Error, tag, logText);
}

bool Log::save(LogType type, LogLevel level, const char *tag, const char *log) {

    // 获取线程名
    char threadName[THREAD_NAME_MAX_LEN] = {0};
    prctl(PR_GET_NAME, threadName);

    // 日志输出到控制台
    LogTools::printLog(level, tag, threadName, "%s", log);

    if (!isInit) {
        LogTools::printLog(level, tag, threadName, ">>> 日志根目录尚未初始化，日志存储失败！");
        return false;
    }

    // 日志存储器
    auto logger = LogImpl::getInstance();
    if (logger == nullptr) {
        if (DEBUG) {
            LogTools::printLog(LogLevel::Error, TAG, threadName, ">>> LogImpl::logger == nullptr !");
        }
        return false;
    }

    // 存储日志
    logger->handleLog(type, level, tag, threadName, log);
    return true;
}