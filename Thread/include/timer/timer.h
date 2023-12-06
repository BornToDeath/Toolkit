//
// Created by lixiaoqing on 2021/8/18.
//

#ifndef THREAD_TIMER_H
#define THREAD_TIMER_H


#include "thread/runnable.h"


class Timer {

public:

    explicit Timer(const Runnable &task) : task_(task) {}

    ~Timer() = default;

    /**
     * 延迟 delayMills 时间后执行
     * @param delay_mills
     * @return
     */
    void StartDelay(unsigned long long delay_mills);

    /**
     * 在 TimeMills 时刻执行
     * @param timeMills
     */
    void StartAt(unsigned long long timeMills);

    /**
     * 取消执行当前任务
     */
    void Cancel();

private:

    Runnable task_;

    bool is_canceled_;

};


#endif //THREAD_TIMER_H
