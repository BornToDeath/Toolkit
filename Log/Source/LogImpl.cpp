//
// Created by lixiaoqing on 2021/5/19.
//

/* 系统头文件 */
#include <iostream>
#include <queue>
#include <cstdio>
#include <cstdarg>
#include <sys/prctl.h>
#include <sstream>
#include <memory>
#include <functional>

/* 自定义头文件 */
#include "LogImpl.h"
#include "LogTools.h"
#include "LogEncryptor.h"

/* 自定义宏 */
#define TAG "LogImpl"


/********** 初始化静态成员变量 ***********/

std::string LogImpl::logRootDir;
LogImpl *LogImpl::logger = nullptr;
std::mutex LogImpl::logMutex;

// 日志处理的条件变量，作为一种同步机制
std::condition_variable LogImpl::logConsumeCondition;


LogImpl *&LogImpl::getInstance() {

    /**
     * 懒加载实现单例。
     * 此处使用了两个if判断语句的技术称为 双检锁。
     * 好处是：只有判断指针为空的时候才加锁，避免每次调用 getSingleton() 方法都加锁，节省锁开销
     */
    if (nullptr == LogImpl::logger) {

        std::unique_lock<std::mutex> lock(LogImpl::logMutex);

        if (nullptr == LogImpl::logger) {

            LogImpl::logger = new LogImpl();
            if (LogImpl::logger == nullptr) {
                return LogImpl::logger;
            }

            /**
             * 另启线程进行日志存储
             */

            auto runnable = []() {
                LogImpl::logger->fetchLogAndWrite();
            };

            // 后台启动线程
            std::thread t1(runnable);
            t1.detach();
        }
    }

    return LogImpl::logger;
}

void LogImpl::releaseSingleton() {
    if (LogImpl::logger) {
        delete LogImpl::logger;
        LogImpl::logger = nullptr;
    }
}

void LogImpl::handleLog(LogType type, LogLevel level, const char *const tag,
                        const char *const threadName, const char *const logText) {

    // 当前时间
    std::string now = LogTools::getCurrentDateTime("%Y-%m-%d %H:%M:%S");

    std::ostringstream oss;
    oss << now << " | "
        << logLevelName[static_cast<int>(level)] << " | "
        << tag << " | "
        << threadName << " | "
        << logText;

    std::string log = oss.str();

    /**
     * 构造日志结构数据并进行处理
     */

    // 构造日志数据结构
    auto logData = std::make_shared<LogData>(type, level, tag, log);

    // 生产者
    {
        // 加锁，保证线程安全
        std::unique_lock<std::mutex> lock(logMutex);

        // 加入到日志队列中
        this->logQueue.push(logData);

        // 通知日志处理线程开始处理日志
        logConsumeCondition.notify_all();
    }
}

bool LogImpl::fetchLogAndWrite() {

    // 设置日志存储线程的线程名
    prctl(PR_SET_NAME, LOG_THREAD_NAME);

    if (DEBUG) {
        char threadName[THREAD_NAME_MAX_LEN] = {0};
        prctl(PR_GET_NAME, threadName);

        LogTools::printLog(LogLevel::Debug, TAG, LOG_THREAD_NAME,
                           ">>> 启动日志存储子线程，线程名：%s，线程 ID：%ld",
                           threadName, std::this_thread::get_id());
    }

    // 死循环处理日志
    while (true) {

        std::shared_ptr<LogData> logData = nullptr;

        // 消费者
        {
            std::unique_lock<std::mutex> lock(logMutex);
//            std::this_thread::sleep_for(std::chrono::seconds(1));

            // 线程阻塞，直到日志队列不为空  TODO:wait函数有可能会导致整个进程无法退出
            logConsumeCondition.wait(lock, [this]() { return !this->logQueue.empty(); });

            // 取日志队列中的头数据
            logData = this->logQueue.front();

            this->logQueue.pop();
        }

        // 处理日志，将日志写入文件
        this->writeLog(logData);
    }

}

bool LogImpl::writeLog(const std::shared_ptr<LogData> &logData) {

    std::string encryptedLogText;

    // 日志加密
    LogEncryptor::encryptLog(logData->getLog(), encryptedLogText);

    encryptedLogText.append("\n");
    logData->setLog(encryptedLogText);

    // 如果 map 中没有对应日志类型的日志存储器，则创建对应的日志存储器并记录到 map 中
    if (this->logContainer.find(logData->getType()) == this->logContainer.end()) {

        std::string logDir = LogImpl::logRootDir + LogTools::getLogTypeName(logData->getType()) + "/";

        if (!LogTools::createDirIfNotExist(logDir.c_str())) {
            if (DEBUG) {
                LogTools::printLog(LogLevel::Error, TAG, LOG_THREAD_NAME,
                                   ">>> 创建日志目录 %s 失败，errno = %d",
                                   logDir.c_str(), errno);
            }
//            Log::setIsInit(false);  // 无需调用此方法，因为如果设置为 false ，则其他类型的日志也无法存储
            return false;
        }

        // 创建 model 并将 model 添加到 map 中
        auto model = std::make_shared<LogModel>(logDir);
        this->logContainer.insert(std::make_pair(logData->getType(), model));
    }

    bool isOk = this->logContainer[logData->getType()]->writeLogToFile(logData);
    return isOk;
}

void LogImpl::setLogRootDir(const std::string &dir) {
    LogImpl::logRootDir = dir;
}