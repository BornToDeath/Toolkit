//
// Created by lixiaoqing on 2021/8/18.
//

#ifndef THREAD_TIMER_H
#define THREAD_TIMER_H


#include "Thread/Runnable.h"


class Timer {

public:

    Timer(const Runnable &task);

    ~Timer() = default;

    /**
     * 延迟 delayMills 时间后执行
     * @param delayMills
     * @return
     */
    void startDelay(unsigned long long delayMills);

    /**
     * 在 timeMills 时刻执行
     * @param timeMills
     */
    void startAt(unsigned long long timeMills);

    /**
     * 取消执行当前任务
     */
    void cancel();

private:

    Runnable task;

    bool isCanceled;

};


#endif //THREAD_TIMER_H
