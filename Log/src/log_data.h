//
// Created by lixiaoqing on 2021/5/19.
//

#ifndef LOG_LOG_DATA_H
#define LOG_LOG_DATA_H

/**
 * 系统头文件
 */
#include <iostream>

/**
 * 自定义头文件
 */
#include "log.h"
#include "log_common.h"

namespace log {

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

    /**
     * 禁用拷贝构造
     * @param log_data
     */
    LogData(const LogData &log_data) = delete;

    /**
     * 禁用赋值运算符
     * @param log_data
     * @return
     */
    LogData &operator=(const LogData &log_data) = delete;

public:

    LogLevel GetLevel() const;

    LogType GetType() const;

    std::string GetTag() const;

    std::string GetLog() const;

    void SetLog(const std::string &log);

/* ======================================================= */
private: /* Fields                                         */
/* ======================================================= */

    /**
     * 日志类型
     */
    LogType type_;

    /**
     * 日志级别
     */
    LogLevel level_;

    /**
     * 日志标签，可以是当前日志所在类名
     */
    std::string tag_;

    /**
     * 日志内容
     */
    std::string log_;

};

}  // namespace log

#endif //LOG_LOG_DATA_H
