//
// Created by lixiaoqing on 2021/5/19.
//

#include "logger.h"

/* 系统头文件 */
#include <sys/prctl.h>

#include <cstdarg>
#include <cstdio>
#include <functional>
#include <iostream>
#include <memory>
#include <queue>
#include <sstream>

/* 自定义头文件 */
#include "log_encryptor.h"
#include "log_tool.h"

/* 自定义宏 */
#define TAG "Logger"

namespace log {

Logger &Logger::Singleton() {
    static Logger logger;
    return logger;
}

bool Logger::Init(const std::string &log_dir) {
    is_quit_ = false;
    log_root_dir_ = log_dir;

    // 写日志
    auto runnable = [this]() {
        // 设置日志存储线程的线程名
        prctl(PR_SET_NAME, LOG_THREAD_NAME);

        if (DEBUG) {
            char thread_name[THREAD_NAME_MAX_LEN] = {0};
            prctl(PR_GET_NAME, thread_name);
            tool::PrintLog(LogLevel::Debug, TAG, LOG_THREAD_NAME,
                           ">>> 启动日志存储子线程, 线程名: %s , 线程 ID: %ld",
                           thread_name, std::this_thread::get_id());
        }

        // 死循环处理日志
        while (!is_quit_) {
            std::shared_ptr<LogData> log_data = nullptr;

            // 消费者
            {
                std::unique_lock<std::mutex> lock(log_mutex_);

                // 线程阻塞，直到日志队列不为空
                log_consume_condition_.wait(lock, [this]() {
                    if (!this->log_queue_.empty() || is_quit_) {
                        return true;
                    }
                    return false;
                });

                // 是否退出循环
                if (is_quit_) {
                    break;
                }

                // 取日志队列中的头数据
                log_data = this->log_queue_.front();
                this->log_queue_.pop();
            }

            // 处理日志，将日志写入文件
            this->WriteLog(log_data);
        }

        // 将队列中剩余日志处理完
        while (!this->log_queue_.empty()) {
            std::shared_ptr<LogData> log = nullptr;
            {
                std::unique_lock<std::mutex> lock(log_mutex_);
                log = this->log_queue_.front();
                this->log_queue_.pop();
            }
            this->WriteLog(log);
        }

        if (DEBUG) {
            tool::PrintLog(LogLevel::Info, TAG, ">>> 日志线程退出!");
        }
    };
    log_thread_ = std::unique_ptr<std::thread>(new std::thread(runnable));
    return true;
}

void Logger::Stop() {
    if (is_quit_) {
        return;
    }
    is_quit_ = true;
    log_consume_condition_.notify_all();
    if (log_thread_) {
        log_thread_->join();
    }
}

void Logger::WriteLog(LogType type, LogLevel level, const char *tag, const char *log) {

    // 退出日志线程时不再处理日志
    if (is_quit_) {
        return;
    }

    // 获取线程名
    char thread_name[THREAD_NAME_MAX_LEN] = {0};
    prctl(PR_GET_NAME, thread_name);

    // 当前时间
    std::string now = tool::GetCurrentDateTime("%Y-%m-%d %H:%M:%S");

    std::ostringstream oss;
    oss << now << " | "
        << logLevelName[static_cast<int>(level)] << " | "
        << tag << " | "
        << thread_name << " | "
        << log;

    /**
     * 构造日志结构数据并进行处理
     */

    // 构造日志数据结构
    auto log_data = std::make_shared<LogData>(type, level, tag, oss.str());

    // 生产者
    {
        // 加锁，保证线程安全
        std::unique_lock<std::mutex> lock(log_mutex_);

        // 加入到日志队列中
        this->log_queue_.push(log_data);

        // 通知日志处理线程开始处理日志
        log_consume_condition_.notify_all();
    }
}

bool Logger::WriteLog(const std::shared_ptr<LogData> &log_data) {

    if (!log_data) {
        return true;
    }

    std::string encrypted_log_text;

    // 日志加密
    LogEncryptor::EncryptLog(log_data->GetLog(), encrypted_log_text);

    encrypted_log_text.append("\n");
    log_data->SetLog(encrypted_log_text);

    // 如果 map 中没有对应日志类型的日志存储器，则创建对应的日志存储器并记录到 map 中
    if (this->log_container_.find(log_data->GetType()) == this->log_container_.end()) {

        std::string log_dir = log_root_dir_ + tool::GetLogTypeName(log_data->GetType()) + "/";

        if (!tool::CreateDirIfNotExist(log_dir.c_str())) {
            if (DEBUG) {
                tool::PrintLog(LogLevel::Error, TAG, LOG_THREAD_NAME,
                               ">>> 创建日志目录 %s 失败，errno = %d",
                               log_dir.c_str(), errno);
            }
//            Log::setIsInit(false);  // 无需调用此方法，因为如果设置为 false ，则其他类型的日志也无法存储
            return false;
        }

        // 创建 model 并将 model 添加到 map 中
        auto model = std::make_shared<LogModel>(log_dir);
        this->log_container_.insert(std::make_pair(log_data->GetType(), model));
    }

    bool isOk = this->log_container_[log_data->GetType()]->WriteLogToFile(log_data);
    return isOk;
}

}  // namespace log