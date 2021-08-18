//
// Created by lixiaoqing on 2021/8/18.
//

#include "Timer/Timer.h"
#include "Thread/SmartThread.h"
#include "Thread/ThreadUtil.h"


Timer::Timer(const Runnable &task) {
    this->task = task;
}

void Timer::startDelay(unsigned long long delayMills) {
    auto thread = SmartThread::newThread([this, delayMills]() {
        Thread::sleep(delayMills);
        this->task();
    });
    thread->setName("Timer");
    if (!isCanceled) {
        thread->start();
    }
}

void Timer::startAt(unsigned long long timeMills) {
    auto now = ThreadUtil::timeMills();
    startDelay(timeMills <= now ? 0 : (timeMills - now));
}

void Timer::cancel() {
    isCanceled = true;
}