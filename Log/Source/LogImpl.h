//
// Created by lixiaoqing on 2021/5/19.
//

#ifndef LOG_LOGIMPL_H
#define LOG_LOGIMPL_H


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

/**
 * 日志实现类。
 * 实现日志存储的核心功能。该类不可被继承
 */
class LogImpl final {

/* ======================================================= */
public: /* Methods                                         */
/* ======================================================= */

    /**
     * 单例模式。获取日志存储器
     * @return 返回的是对象的指针的引用
     */
    static LogImpl *&getInstance();

    /**
     * 释放单例
     */
    void releaseSingleton();

    /**
     * 设置日志根目录
     * @param dir
     */
    static void setLogRootDir(const std::string &dir);

    /**
     * 日志存储
     * 包括：1. 格式化日志
     *      2. 将日志加入到待存储队列中
     * @param type       日志类型
     * @param level      日志级别
     * @param tag        标签
     * @param threadName 线程名
     * @param logText    日志内容
     */
    void handleLog(LogType type, LogLevel level, const char *tag, const char *threadName, const char *logText);

    /**
     * 构造函数。禁止外部构造
     */
    LogImpl() = default;

    /**
     * 析构函数。禁止外部析构
     */
    ~LogImpl() = default;

    /**
     * 禁用拷贝构造函数
     * @param log
     */
    LogImpl(const LogImpl &log) = delete;

    /**
     * 禁用赋值构造函数
     * @param log
     * @return
     */
    LogImpl &operator=(const LogImpl &log) = delete;

private:

    /**
     * 将单条日志写入文件
     * @param logData
     * @return
     */
    bool writeLog(const std::shared_ptr<LogData> &logData);

    /**
     * 处理日志。从日志队列中取日志数据并进行存储
     * @return
     */
    bool fetchLogAndWrite();


/* ======================================================= */
private: /* Fields                                         */
/* ======================================================= */

    /**
     * 日志根目录
     */
    static std::string logRootDir;

    /**
     * 日志存储器。单例
     * 注意：单例下不可使用 shared_ptr
     */
    static LogImpl *logger;

    /**
     * 锁
     */
    static std::mutex logMutex;

    /**
     * 日志处理的条件变量，作为一种同步机制
     */
    static std::condition_variable logConsumeCondition;

    /**
     * 日志队列。用于存放待存储的日志数据
     */
    std::queue<std::shared_ptr<LogData>> logQueue;

    /**
     * 容器，存放不同类型的日志存储器
     */
    std::map<LogType, std::shared_ptr<LogModel>> logContainer;

    /**
     * 日志线程是否退出的标志
     */
    static bool isQuit;
};


#endif //LOG_LOGIMPL_H
