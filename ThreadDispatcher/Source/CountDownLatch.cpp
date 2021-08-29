//
// Created by lixiaoqing on 2021/8/29.
//

#include "CountDownLatch.h"


CountDownLatch::CountDownLatch(int count) : count(count) {

}

std::shared_ptr<CountDownLatch> CountDownLatch::create(int count) {
    return std::make_shared<CountDownLatch>(count);
}

void CountDownLatch::countDown() {
    {
        std::unique_lock<std::recursive_mutex> lock(mutex);
        --count;
        if (count <= 0) {
            helper.notify();
        }
    }
}

void CountDownLatch::await() {
    auto pred = [this]() -> bool {
        return this->count <= 0;
    };
    helper.waitUntil(pred);
}