//
// Created by lixiaoqing on 2021/8/29.
//

#include "lock/wait_helper.h"

#include <utility>

void WaitHelper::WaitUntil(std::function<bool()> prediction) {
    {
        std::unique_lock<std::mutex> lock(mutex_);
        condition_.wait(lock, std::move(prediction));
    }
}

bool WaitHelper::WaitFor(unsigned long long mills, std::function<bool()> prediction) {
    {
        std::unique_lock<std::mutex> lock(mutex_);
        auto ret = condition_.wait_for(lock, std::chrono::milliseconds(mills), std::move(prediction));
        return ret;
    }
}

void WaitHelper::Notify() {
    condition_.notify_all();
}

void WaitHelper::Lock() {
    mutex_.lock();
}

void WaitHelper::Unlock() {
    mutex_.unlock();
}

bool WaitHelper::TryLock() {
    return mutex_.try_lock();
}