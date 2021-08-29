//
// Created by lixiaoqing on 2021/8/29.
//

#ifndef THREADDISPATCHER_THREADS_H
#define THREADDISPATCHER_THREADS_H


#include <map>
#include "Thread/HandlerThread.h"


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
    static void runOnThread(const std::string &name, Runnable runnable, const std::string &tag = "Default");

    /**
     * 运行在指定线程。
     * 如果当前线程就是指定的线程，会 POST 到消息队列，等待执行。
     * 如果线程不存在，会创建一个新线程。
     */
    static void postOnThread(const std::string &name, Runnable runnable, Timestamp delayMills = 0,
                             const std::string &tag = "Default");

    /**
     * 释放指定的线程
     */
    static void releaseThread(const std::string &name);

public:

    /**
     * 默认异步串行线程池。
     */
    static const std::string SERIAL_QUEUE;

    /**
     * 默认异步并行线程池。
     */
    static const std::string CONCURRENT_QUEUE;

private:

    /**
     * 线程集合
     */
    static std::map<std::string, HandlerThread *> threads;

    /**
     * threads 的锁
     */
    static std::recursive_mutex nameMutex;
};


#endif //THREADDISPATCHER_THREADS_H
