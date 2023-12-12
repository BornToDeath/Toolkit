//
// Created by lixiaoqing on 2021/8/19.
//

#ifndef THREAD_DISPATCHER_THREAD_DISPATCHER_H
#define THREAD_DISPATCHER_THREAD_DISPATCHER_H

#include "thread/handler_thread.h"
#include "thread/runnable.h"
#include "thread/thread_local.h"
#include "thread/thread_pool.h"
#include "thread/thread_util.h"


/**
 * 线程分发器
 */
class ThreadDispatcher {

public:

    ThreadDispatcher(const std::string &name, size_t max_concurrent_thread_count);

    ~ThreadDispatcher();

public:

    /**
     * 获取默认的 ThreadDispatcher
     * @return
     */
    static ThreadDispatcher *GetDefaultThreadDispatcher();

    /**
     * 在 Bus 线程执行
     * @param runnable
     */
    void RunOnBus(Runnable runnable);

    /**
     * 在 Bus 线程延迟执行
     * @param runnable
     * @param delay
     */
    void PostOnBus(Runnable runnable, Timestamp delay);

    /**
     * 在 串行异步线程池 中执行
     * @param runnable
     * @param name
     */
    void RunOnSerial(Runnable runnable, std::string name = "NoName");

    /**
     * 在 并发异步线程池 中执行
     * @param runnable
     * @param name
     */
    void RunOnConcurrent(Runnable runnable, std::string name = "NoName");

    /**
     * 获取当前线程的 Flag
     * @return
     */
    int GetCurrentThreadFlag();

public:

    /** 线程标记：BUS 线程 */
    static const int kThreadFlagBus = 1;

    /** 线程标记：Serial 线程 */
    static const int kThreadFlagSerial = 2;

    /** 线程标记：Concurrent 线程 */
    static const int kThreadFlagConcurrent = 3;

private:

    /**
     * 默认的 ThreadDispatcher 对象
     */
    static ThreadDispatcher *default_dispatcher_;

    /**
     * 总线线程（其实就是个 HandlerThread ）
     */
    HandlerThread *bus_thread_;

    /**
     * 串行异步线程池（线程池中只有一个线程）
     */
    ThreadPool *serial_pool_;

    /**
     * 并发异步线程池（线程池中有多个线程）
     */
    ThreadPool *concurrent_pool_;

    /**
     * 线程的标记
     */
    ThreadLocal<int> thread_flags_;

};


#endif //THREAD_DISPATCHER_THREAD_DISPATCHER_H
