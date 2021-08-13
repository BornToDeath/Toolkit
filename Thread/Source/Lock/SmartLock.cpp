//
// Created by lixiaoqing on 2021/8/12.
//

#include "Lock/SmartLock.h"
#include "Thread/ThreadLog.h"

#define TAG "SmartLock"


SmartLock::SmartLock(std::mutex &mutex) : mMutex(&mutex) {
    if (mMutex) {
        mMutex->lock();
    }
}

SmartLock::SmartLock(std::mutex &mutex, const std::string name) : mMutex(&mutex) {
    if (mMutex) {
        this->name = name;
        logger.i(TAG, name + " locked.");
        mMutex->lock();
    }
}

SmartLock::~SmartLock() {
    if (mMutex) {
        mMutex->unlock();
        if (!this->name.empty()) {
            logger.i(TAG, name + " unlocked.");
        }
    }
}
