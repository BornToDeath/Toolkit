//
// Created by lixiaoqing on 2021/8/29.
//

#ifndef THREAD_DISPATCHER_COUNTDOWN_LATCH_H
#define THREAD_DISPATCHER_COUNTDOWN_LATCH_H

#include "lock/wait_helper.h"


/**
 * 类似于 Java 中的 CountDownLatch，用于等待多个并行任务都执行完毕再执行下一步操作。
 */
class CountDownLatch {

public:

    /**
     * @param count 等待的数量
     */
    explicit CountDownLatch(int count);

    ~CountDownLatch() = default;

    CountDownLatch(const CountDownLatch &) = delete;

    CountDownLatch &operator=(const CountDownLatch &) = delete;

public:

    /**
     * 创建智能指针实例。
     */
    static std::shared_ptr<CountDownLatch> Create(int count);

    /**
     * 让等待数量减一，意味着有一个任务已经完成了。
     */
    void CountDown();

    /**
     * 等待直到任务完成。
     */
    void Await();

private:

    int count_;

    std::recursive_mutex mutex_;

    WaitHelper helper_;
};


#endif //THREAD_DISPATCHER_COUNTDOWN_LATCH_H
