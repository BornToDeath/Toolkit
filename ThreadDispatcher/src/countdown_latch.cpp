//
// Created by lixiaoqing on 2021/8/29.
//

#include "countdown_latch.h"


CountDownLatch::CountDownLatch(int count) : count_(count) {}

std::shared_ptr<CountDownLatch> CountDownLatch::Create(int count) {
    return std::make_shared<CountDownLatch>(count);
}

void CountDownLatch::CountDown() {
    {
        std::unique_lock<std::recursive_mutex> lock(mutex_);
        --count_;
        if (count_ <= 0) {
            helper_.Notify();
        }
    }
}

void CountDownLatch::Await() {
    auto pred = [this]() -> bool {
        return this->count_ <= 0;
    };
    helper_.WaitUntil(pred);
}