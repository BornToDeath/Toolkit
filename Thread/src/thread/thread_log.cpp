//
// Created by lixiaoqing on 2021/8/12.
//

#include "thread/thread_log.h"

#include <sys/prctl.h>

#include <chrono>
#include <cstdarg>
#include <cstdio>
#include <utility>

// 是否打印日志
#define PRINT_LOG 0

// 线程库全局的 logger
ThreadLog logger;


ThreadLog::ThreadLog() {
    this->core_ = [](const int level, const std::string &tag, const std::string &log) {
        if (PRINT_LOG) {
            // 线程名
            const int size = 32;
            char thread_name[size] = {};
            prctl(PR_GET_NAME, thread_name);

            // 日期
            char now[size] = {};
            auto time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
            strftime(now, size, "%Y-%m-%d %H:%M:%S", std::localtime(&time));

            printf("%s | %s | %s | %s\n", now, thread_name, tag.c_str(), log.c_str());
        }
    };
}

void ThreadLog::v(const std::string &tag, const std::string &msg, ...) const {
    if (core_) {
        const int LEN = 1024;
        char log[LEN] = {0};
        va_list args;
        va_start(args, msg);
        vsnprintf(log, LEN, msg.c_str(), args);
        va_end(args);
        core_(0, tag, msg);
    }
}

void ThreadLog::i(const std::string &tag, const std::string &msg, ...) const {
    if (core_) {
        const int LEN = 1024;
        char log[LEN] = {0};
        va_list args;
        va_start(args, msg);
        vsnprintf(log, LEN, msg.c_str(), args);
        va_end(args);
        core_(1, tag, log);
    }
}
