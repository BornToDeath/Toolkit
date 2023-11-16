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
#include "logger.h"
#include "log_tool.h"
#include "log_encryptor.h"

/* 自定义宏 */
#define TAG "Logger"

namespace log {

Logger &Logger::Singleton() {
    static Logger logger;
    return logger;
}

bool Logger::Init(const std::string &logDir) {
    isQuit_ = false;
    logRootDir_ = logDir;

    // 写日志
    auto runnable = [this]() {
        // 设置日志存储线程的线程名
        prctl(PR_SET_NAME, LOG_THREAD_NAME);

        if (DEBUG) {
            char threadName[THREAD_NAME_MAX_LEN] = {0};
            prctl(PR_GET_NAME, threadName);
            tool::PrintLog(LogLevel::Debug, TAG, LOG_THREAD_NAME,
                           ">>> 启动日志存储子线程, 线程名: %s , 线程 ID: %ld",
                           threadName, std::this_thread::get_id());
        }

        // 死循环处理日志
        while (!isQuit_) {
            std::shared_ptr<LogData> logData = nullptr;

            // 消费者
            {
                std::unique_lock<std::mutex> lock(logMutex_);

                // 线程阻塞，直到日志队列不为空
                logConsumeCondition_.wait(lock, [this]() {
                    if (!this->logQueue_.empty() || isQuit_) {
                        return true;
                    }
                    return false;
                });

                // 是否退出循环
                if (isQuit_) {
                    break;
                }

                // 取日志队列中的头数据
                logData = this->logQueue_.front();
                this->logQueue_.pop();
            }

            // 处理日志，将日志写入文件
            this->WriteLog(logData);
        }

        // 将队列中剩余日志处理完
        while (!this->logQueue_.empty()) {
            std::shared_ptr<LogData> log = nullptr;
            {
                std::unique_lock<std::mutex> lock(logMutex_);
                log = this->logQueue_.front();
                this->logQueue_.pop();
            }
            this->WriteLog(log);
        }

        if (DEBUG) {
            tool::PrintLog(LogLevel::Info, TAG, ">>> 日志线程退出!");
        }
    };
    logThread_ = std::unique_ptr<std::thread>(new std::thread(runnable));
    return true;
}

void Logger::Stop() {
    if (isQuit_) {
        return;
    }
    isQuit_ = true;
    logConsumeCondition_.notify_all();
    if (logThread_) {
        logThread_->join();
    }
}

void Logger::WriteLog(LogType type, LogLevel level, const char *tag, const char *log) {

    // 退出日志线程时不再处理日志
    if (isQuit_) {
        return;
    }

    // 获取线程名
    char threadName[THREAD_NAME_MAX_LEN] = {0};
    prctl(PR_GET_NAME, threadName);

    // 当前时间
    std::string now = tool::GetCurrentDateTime("%Y-%m-%d %H:%M:%S");

    std::ostringstream oss;
    oss << now << " | "
        << logLevelName[static_cast<int>(level)] << " | "
        << tag << " | "
        << threadName << " | "
        << log;

    /**
     * 构造日志结构数据并进行处理
     */

    // 构造日志数据结构
    auto logData = std::make_shared<LogData>(type, level, tag, oss.str());

    // 生产者
    {
        // 加锁，保证线程安全
        std::unique_lock<std::mutex> lock(logMutex_);

        // 加入到日志队列中
        this->logQueue_.push(logData);

        // 通知日志处理线程开始处理日志
        logConsumeCondition_.notify_all();
    }
}

bool Logger::WriteLog(const std::shared_ptr<LogData> &logData) {

    if (!logData) {
        return true;
    }

    std::string encryptedLogText;

    // 日志加密
    LogEncryptor::EncryptLog(logData->GetLog(), encryptedLogText);

    encryptedLogText.append("\n");
    logData->SetLog(encryptedLogText);

    // 如果 map 中没有对应日志类型的日志存储器，则创建对应的日志存储器并记录到 map 中
    if (this->logContainer_.find(logData->GetType()) == this->logContainer_.end()) {

        std::string logDir = logRootDir_ + tool::GetLogTypeName(logData->GetType()) + "/";

        if (!tool::CreateDirIfNotExist(logDir.c_str())) {
            if (DEBUG) {
                tool::PrintLog(LogLevel::Error, TAG, LOG_THREAD_NAME,
                               ">>> 创建日志目录 %s 失败，errno = %d",
                               logDir.c_str(), errno);
            }
//            Log::setIsInit(false);  // 无需调用此方法，因为如果设置为 false ，则其他类型的日志也无法存储
            return false;
        }

        // 创建 model 并将 model 添加到 map 中
        auto model = std::make_shared<LogModel>(logDir);
        this->logContainer_.insert(std::make_pair(logData->GetType(), model));
    }

    bool isOk = this->logContainer_[logData->GetType()]->WriteLogToFile(logData);
    return isOk;
}

}  // namespace log