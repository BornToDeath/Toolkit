//
// Created by lixiaoqing on 2021/8/29.
//

#include "Lock/WaitHelper.h"


WaitHelper::WaitHelper() : isWaiting_(false) {

}

void WaitHelper::waitUntil(std::function<bool()> prediction) {
    {
        std::unique_lock<std::mutex> lock(mutex);
        condition.wait(lock, prediction);
    }
}

bool WaitHelper::waitFor(unsigned long long mills, std::function<bool()> prediction) {
    {
        std::unique_lock<std::mutex> lock(mutex);
        auto ret = condition.wait_for(lock, std::chrono::milliseconds(mills), prediction);
        return ret;
    }
}

bool WaitHelper::isWaiting() const {
    return isWaiting_;
}

void WaitHelper::notify() {
    condition.notify_all();
}

void WaitHelper::lock() {
    mutex.lock();
}

void WaitHelper::unlock() {
    mutex.unlock();
}