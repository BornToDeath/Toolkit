//
// Created by lixiaoqing on 2021/8/13.
//

#include "lock/smart_recursive_lock.h"

#include "thread/thread_log.h"

#define TAG "SmartRecursiveLock"


SmartRecursiveLock::SmartRecursiveLock(std::recursive_mutex &mutex) : mutex_(&mutex) {
    if (mutex_) {
        mutex_->lock();
    }
}

SmartRecursiveLock::SmartRecursiveLock(std::recursive_mutex &mutex, const std::string &name) : mutex_(&mutex),
                                                                                               name_(name) {
    if (mutex_) {
        mutex_->lock();
        if (!name_.empty()) {
            logger.i(TAG, name_ + " locked.");
        }
    }
}

SmartRecursiveLock::~SmartRecursiveLock() {
    if (mutex_) {
        mutex_->unlock();
        if (!name_.empty()) {
            logger.i(TAG, name_ + " unlocked.");
        }
    }
}