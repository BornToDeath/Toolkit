//
// Created by lixiaoqing on 2021/8/12.
//

#include <chrono>
#include <sys/prctl.h>
#include "Thread/ThreadLog.h"

// 是否打印日志
#define PRINT_LOG 1

// 线程库全局的 logger
ThreadLog logger;


ThreadLog::ThreadLog() {
    this->core = [](int level, std::string tag, std::string msg) {
        if (PRINT_LOG) {
            // 线程名
            const int size = 32;
            char buf[size] = {};
            prctl(PR_GET_NAME, buf);

            // 日期
            char now[size] = {};
            auto time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
            strftime(now, size, "%Y-%m-%d %H:%M:%S", std::localtime(&time));

            printf("%s | %s | %s | %s\n", now, buf, tag.c_str(), msg.c_str());
        }
    };
}

ThreadLog::ThreadLog(std::function<void(int, std::string, std::string)> core) {
    this->core = core;
}

void ThreadLog::v(const std::string &tag, const std::string &msg) {
    if (core) {
        core(0, tag, msg);
    }
}

void ThreadLog::i(const std::string &tag, const std::string &msg) {
    if (core) {
        core(2, tag, msg);
    }
}
