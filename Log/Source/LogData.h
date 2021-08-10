//
// Created by lixiaoqing on 2021/5/19.
//

#ifndef LOG_LOGDATA_H
#define LOG_LOGDATA_H

/**
 * 系统头文件
 */
#include <iostream>

/**
 * 自定义头文件
 */
#include "LogCommon.h"
#include "Log.h"


/**
 * 日志数据结构类。
 * 该类不可被继承
 */
class LogData final {

/* ======================================================= */
public: /* Methods                                         */
/* ======================================================= */

    /**
     * 构造函数
     * @param type  日志类型
     * @param level 日志级别
     * @param tag   日志标签
     * @param log   日志内容
     */
    LogData(LogType type, LogLevel level, const std::string &tag, std::string log);

    ~LogData() = default;

    LogLevel getLevel() const;

    LogType getType() const;

    std::string getTag() const;

    std::string getLog() const;

    void setLog(const std::string &log);

    /**
     * 禁用拷贝构造
     * @param logData
     */
    LogData(const LogData &logData) = delete;

    /**
     * 禁用赋值运算符
     * @param logData
     * @return
     */
    LogData &operator=(const LogData &logData) = delete;

/* ======================================================= */
private: /* Fields                                         */
/* ======================================================= */

    /**
     * 日志类型
     */
    LogType type;

    /**
     * 日志级别
     */
    LogLevel level;

    /**
     * 日志标签，可以是当前日志所在类名
     */
    std::string tag;

    /**
     * 日志内容
     */
    std::string log;

};


#endif //LOG_LOGDATA_H
