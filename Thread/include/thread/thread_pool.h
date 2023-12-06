//
// Created by lixiaoqing on 2021/8/17.
//

#ifndef THREAD_THREAD_POOL_H
#define THREAD_THREAD_POOL_H


#include "thread/leak_companion.h"
#include "thread/runnable.h"
#include "thread/smart_loop_thread.h"


class ThreadPool {

public:

    static ThreadPool *NewInstance(std::string name, const size_t max_thread_count);

    static ThreadPool *NewInstance(std::string name, const size_t maxThreadCount, Runnable initialRunnable);

    /**
     * 执行一个任务
     */
    void Execute(std::string name, Runnable runnable);

    /**
     * 获取当前 正在工作的 和 空闲的线程 的总数
     */
    size_t GetThreadCount();

    /**
     * 释放所有空闲的线程
     */
    void ReleaseIdleThreads();

    /**
     * 关闭所有的线程，并释放占用的内存。
     *
     * 调用这个函数后，调用方需要把指针置为 nullptr。
     */
    void ShutDownAndDeleteSelf();

private:

    explicit ThreadPool(std::string name, size_t maxThreadCount, Runnable initialRunnable);

    virtual ~ThreadPool() = default;

    /**
     * 尝试执行任务队列中的任务
     *
     * 触发：
     *     1. 调用 execute(.) 方法后触发一次;
     *     2. 每个线程执行完自己的任务后会触发一次。
     *
     * 线程安全:
     *     1. 保证整个函数 Thread 数组的线程安全
     */
    void TryExecute();

    /**
     * 用空闲线程执行队列中的第一个任务
     *
     * 注意：内部不保证线程安全，由调用的地方保证
     */
    void ExecuteWithIdleThread();

    /**
     * 创建新的线程
     * 内部不保证线程安全
     */
    void CreateNewThread();

    /**
     * 生成下一个线程的名字
     *
     * @param name [OUT]
     */
    void GenerateNextThreadName(std::string &name);

    /**
     * 是否还有待执行的任务
     */
    bool HasTask();


private:

    ADD_LEAK_DETECTOR

    /**
     * 线程池的名称
     */
    std::string name_;

    /**
     * 线程执行的第一个 Runnable.
     */
    Runnable init_runnable_;

    /**
     * 线程池是否工作
     */
    bool is_running_;

    /**
     * 空闲的线程
     */
    std::vector<SmartLoopThread *> idle_threads_;

    /**
     * 工作中的线程
     */
    std::vector<SmartLoopThread *> working_threads_;

    /**
     * 待执行的任务
     */
    std::queue<RunnableWrapper> tasks_;

    /**
     * 保证 mIdleThreads、 mWorkingThreads 线程安全的锁
     */
    std::recursive_mutex thread_mutex_;

    /**
     * 保证 mTasks 线程安全的锁
     */
    std::recursive_mutex task_mutex_;

    /**
     * 最大的线程数
     */
    size_t max_thread_count_;

    /**
     * 已经创建过的线程的数量
     */
    size_t created_thread_count_;
};


#endif //THREAD_THREAD_POOL_H
