//
// Created by lixiaoqing on 2021/5/19.
//

#ifndef LOG_LOGGER_H
#define LOG_LOGGER_H


/**
 * 系统头文件
 */
#include <thread>
#include <map>
#include <mutex>
#include <condition_variable>

/**
 * 自定义头文件
 */
#include "LogMmap.h"
#include "LogData.h"
#include "Log.h"
#include "LogModel.h"

namespace log {

/**
 * 日志实现类。
 * 实现日志存储的核心功能。该类不可被继承
 */
class Logger final {

private:
    /**
     * 构造函数。禁止外部构造
     */
    Logger() = default;

    /**
     * 析构函数。禁止外部析构
     */
    ~Logger() = default;

    /**
     * 禁用拷贝构造函数
     * @param log
     */
    Logger(const Logger &logger) = delete;

    /**
     * 禁用赋值构造函数
     * @param log
     * @return
     */
    Logger &operator=(const Logger &logger) = delete;

/* ======================================================= */
public: /* Methods                                         */
/* ======================================================= */

    /**
     * 单例模式。获取日志存储器
     * @return 返回的是对象的指针的引用
     */
    static Logger &Singleton();

    /**
     * 设置日志根目录
     * @param dir
     */
    bool Init(const std::string &logDir);

    /**
     * 停止写日志
     */
    void Stop();

    /**
     * 日志存储
     * 包括：1. 格式化日志
     *      2. 将日志加入到待存储队列中
     * @param type       日志类型
     * @param level      日志级别
     * @param tag        标签
     * @param log        日志内容
     */
    void WriteLog(LogType type, LogLevel level, const char *tag, const char *log);

private:

    /**
     * 将单条日志写入文件
     * @param logData
     * @return
     */
    bool WriteLog(const std::shared_ptr<LogData> &logData);


/* ======================================================= */
private: /* Fields                                         */
/* ======================================================= */

    /**
     * 日志根目录
     */
    std::string logRootDir_;

    /**
     * 锁
     */
    std::mutex logMutex_;

    /**
     * 日志处理的条件变量，作为一种同步机制
     */
    std::condition_variable logConsumeCondition_;

    /**
     * 日志队列。用于存放待存储的日志数据
     */
    std::queue<std::shared_ptr<LogData>> logQueue_;

    /**
     * 容器，存放不同类型的日志存储器
     */
    std::map<LogType, std::shared_ptr<LogModel>> logContainer_;

    /**
     * 日志线程是否退出的标志
     */
    bool isQuit_{true};

    // 写日志线程
    std::unique_ptr<std::thread> logThread_;
};

}  // namespace log

#endif //LOG_LOGGER_H
