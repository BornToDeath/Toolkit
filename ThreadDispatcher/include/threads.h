//
// Created by lixiaoqing on 2021/8/29.
//

#ifndef THREAD_DISPATCHER_THREADS_H
#define THREAD_DISPATCHER_THREADS_H

#include <map>

#include "thread/handler_thread.h"

/**
 * 线程列表
 */
class Threads {

public:

    /**
     * 运行在指定线程。
     * 如果当前线程就是指定的线程，会立刻执行。
     * 如果线程不存在，会创建一个新线程。
     *
     * @param name 线程名
     * @param tag  这个 runnable 的标记
     */
    static void RunOnThread(const std::string &name, Runnable runnable, const std::string &tag = "Default");

    /**
     * 运行在指定线程。
     * 如果当前线程就是指定的线程，会 POST 到消息队列，等待执行。
     * 如果线程不存在，会创建一个新线程。
     */
    static void PostOnThread(const std::string &name, Runnable runnable, Timestamp delay_mills = 0, const std::string &tag = "Default");

    /**
     * 释放指定的线程
     */
    static void ReleaseThread(const std::string &name);

public:

    /**
     * 默认异步串行线程池。
     */
    static const std::string kSerialQueue;

    /**
     * 默认异步并行线程池。
     */
    static const std::string kConcurrentQueue;

private:

    /**
     * 线程集合
     */
    static std::map<std::string, HandlerThread *> threads_;

    /**
     * threads_ 的锁
     */
    static std::recursive_mutex threads_mutex_;
};


#endif //THREAD_DISPATCHER_THREADS_H
