//
// Created by lixiaoqing on 2021/8/29.
//

#ifndef THREADDISPATCHER_COUNTDOWNLATCH_H
#define THREADDISPATCHER_COUNTDOWNLATCH_H

#include "Lock/WaitHelper.h"


/**
 * 类似于 Java 中的 CountDownLatch，用于等待多个并行任务都执行完毕再执行下一步操作。
 */
class CountDownLatch {

public:

    /**
     * @param count 等待的数量
     */
    CountDownLatch(int count);

    ~CountDownLatch() = default;

    CountDownLatch(const CountDownLatch &) = delete;

    CountDownLatch &operator=(const CountDownLatch &) = delete;

public:

    /**
     * 创建智能指针实例。
     */
    static std::shared_ptr<CountDownLatch> create(int count);

    /**
     * 让等待数量减一，意味着有一个任务已经完成了。
     */
    void countDown();

    /**
     * 等待直到任务完成。
     */
    void await();

private:

    int count;

    std::recursive_mutex mutex;

    WaitHelper helper;
};


#endif //THREADDISPATCHER_COUNTDOWNLATCH_H
