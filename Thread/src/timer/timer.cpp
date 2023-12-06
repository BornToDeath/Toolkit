//
// Created by lixiaoqing on 2021/8/18.
//

#include "timer/timer.h"

#include "thread/smart_thread.h"
#include "thread/thread_util.h"

void Timer::StartDelay(unsigned long long delay_mills) {
    auto thread = SmartThread::NewInstance([this, delay_mills]() {
        Thread::Sleep(delay_mills);
        if (!is_canceled_) {
            this->task_();
        }
    });
    thread->SetName("Timer");
    thread->Start();
}

void Timer::StartAt(unsigned long long timeMills) {
    auto now = ThreadUtil::TimeMills();
    StartDelay(timeMills <= now ? 0 : (timeMills - now));
}

void Timer::Cancel() {
    is_canceled_ = true;
}