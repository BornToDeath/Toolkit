//
// Created by lixiaoqing on 2021/8/12.
//

#include "lock/smart_lock.h"

#include "thread/thread_log.h"

#define TAG "SmartLock"


SmartLock::SmartLock(std::mutex &mutex) : mutex_(&mutex) {
    if (mutex_) {
        mutex_->lock();
    }
}

SmartLock::SmartLock(std::mutex &mutex, const std::string &name) : mutex_(&mutex), name_(name) {
    if (mutex_) {
        mutex_->lock();
        logger.i(TAG, name + " locked.");
    }
}

SmartLock::~SmartLock() {
    if (mutex_) {
        mutex_->unlock();
        if (!this->name_.empty()) {
            logger.i(TAG, name_ + " unlocked.");
        }
    }
}
