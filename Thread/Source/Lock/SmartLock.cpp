//
// Created by lixiaoqing on 2021/8/12.
//

#include "Lock/SmartLock.h"
#include "Thread/ThreadLog.h"

#define TAG "SmartLock"


SmartLock::SmartLock() {
    mutex.lock();
}

SmartLock::SmartLock(const std::string name) {
    this->name = name;
    logger.i(TAG, name + " locked");
    mutex.lock();
}

SmartLock::~SmartLock() {
    mutex.unlock();
    if (!this->name.empty()) {
        logger.i(TAG, name + " unlocked");
    }
}
