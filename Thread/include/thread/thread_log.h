//
// Created by lixiaoqing on 2021/8/12.
//

#ifndef THREAD_THREAD_LOG_H
#define THREAD_THREAD_LOG_H

#include <functional>
#include <iostream>
#include <utility>


class ThreadLog {
    using Logger = std::function<void(const int level, const std::string &tag, const std::string &log)>;

public:

    ThreadLog();

    explicit ThreadLog(Logger core) : core_(std::move(core)) {}

public:

    void i(const std::string &tag, const std::string &msg, ...) const;

    void v(const std::string &tag, const std::string &msg, ...) const;

private:

    Logger core_;

};

/**
 * 线程库全局的 logger
 */
extern ThreadLog logger;


#endif //THREAD_THREAD_LOG_H
