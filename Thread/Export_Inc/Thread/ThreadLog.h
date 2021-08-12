//
// Created by lixiaoqing on 2021/8/12.
//

#ifndef THREAD_THREADLOG_H
#define THREAD_THREADLOG_H

#include <iostream>
#include <functional>


class ThreadLog {

public:

    ThreadLog();

    explicit ThreadLog(std::function<void(int level, std::string, std::string)> core);

public:

    void i(const std::string &tag, const std::string &msg);

    void v(const std::string &tag, const std::string &msg);

private:

    std::function<void(int level, std::string, std::string)> core;

};

/**
 * 线程库全局的 logger
 */
extern ThreadLog logger;


#endif //THREAD_THREADLOG_H
