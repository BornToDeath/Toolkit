//
// Created by lixiaoqing on 2021/8/19.
//

#ifndef THREAD_THREADDISPATCHER_H
#define THREAD_THREADDISPATCHER_H

#include "Thread/Runnable.h"
#include "Thread/HandlerThread.h"
#include "Thread/ThreadLocal.h"
#include "Thread/ThreadUtil.h"
#include "ThreadPool/ThreadPool.h"


/**
 * 线程分发器
 */
class ThreadDispatcher {

public:

    ThreadDispatcher(const std::string &name, size_t maxConcurrentThreadNum);

    ~ThreadDispatcher();

public:

    /**
     * 获取默认的 ThreadDispatcher
     * @return
     */
    static ThreadDispatcher *getDefaultThreadDispatcher();

    /**
     * 在 Bus 线程执行
     * @param runnable
     */
    void runOnBus(Runnable runnable);

    /**
     * 在 Bus 线程延迟执行
     * @param runnable
     * @param delay
     */
    void postOnBus(Runnable runnable, Timestamp delay);

    /**
     * 在 串行异步线程池 中执行
     * @param runnable
     * @param name
     */
    void runOnSerial(Runnable runnable, std::string name = "NoName");

    /**
     * 在 并发异步线程池 中执行
     * @param runnable
     * @param name
     */
    void runOnConcurrent(Runnable runnable, std::string name = "NoName");

    /**
     * 获取当前线程的 Flag
     * @return
     */
    int getCurrentThreadFlag();

public:

    /** 线程标记：BUS 线程 */
    static const int THREAD_FLAG_BUS = 1;

    /** 线程标记：Serial 线程 */
    static const int THREAD_FLAG_SERIAL = 2;

    /** 线程标记：Concurrent 线程 */
    static const int THREAD_FLAG_CONCURRENT = 3;

private:

    /**
     * 默认的 ThreadDispatcher 对象
     */
    static ThreadDispatcher *defaultDispatcher;

    /**
     * 总线线程（其实就是个 HandlerThread ）
     */
    HandlerThread *busThread;

    /**
     * 串行异步线程池（线程池中只有一个线程）
     */
    ThreadPool *serialPool;

    /**
     * 并发异步线程池（线程池中有多个线程）
     */
    ThreadPool *concurrentPool;

    /**
     * 线程的标记
     */
    ThreadLocal<int> threadFlags;

};


#endif //THREAD_THREADDISPATCHER_H
