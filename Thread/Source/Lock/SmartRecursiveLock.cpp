//
// Created by lixiaoqing on 2021/8/13.
//

#include "Lock/SmartRecursiveLock.h"
#include "Thread/ThreadLog.h"

#define TAG "SmartRecursiveLock"


SmartRecursiveLock::SmartRecursiveLock(std::recursive_mutex &mutex) : mMutex(&mutex) {
    if (mMutex) {
        mMutex->lock();
    }
}

SmartRecursiveLock::SmartRecursiveLock(std::recursive_mutex &mutex, const std::string &name) : mMutex(&mutex) {
    if (mMutex) {
        if (!name.empty()) {
            this->name = name;
//            logger.i(TAG, name + " locked.");
        }
        mMutex->lock();
    }
}

SmartRecursiveLock::~SmartRecursiveLock() {
    if (mMutex) {
        mMutex->unlock();
        if (!name.empty()) {
//            logger.i(TAG, name + " unlocked.");
        }
    }
}