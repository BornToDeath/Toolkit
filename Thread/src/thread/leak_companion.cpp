//
// Created by lixiaoqing on 2021/8/12.
//

#include "thread/leak_companion.h"
#include "thread/thread_log.h"

#define TAG "LeakCompanion"

std::vector<LeakCompanion *> LeakCompanion::companions_;
std::recursive_mutex LeakCompanion::mutex_;


LeakCompanion::LeakCompanion() {
    name_ = "Default";
//    logger.i(TAG, "LeakCompanion ++, name=" + name);

    mutex_.lock();
    companions_.push_back(this);
    mutex_.unlock();
}

LeakCompanion::LeakCompanion(const std::string &name) : name_(name) {
//    logger.i(TAG, "LeakCompanion ++, name=" + name);

    mutex_.lock();
    companions_.push_back(this);
    mutex_.unlock();
}

LeakCompanion::~LeakCompanion() {
//    logger.i(TAG, "LeakCompanion --, name=" + name);
    mutex_.lock();
    auto it = companions_.begin();
    while (it != companions_.end()) {
        if (*it == this) {
            companions_.erase(it);
            break;
        }
        it++;
    }
    mutex_.unlock();
}

void LeakCompanion::SetName(std::string name) {
    this->name_ = std::move(name);
}

size_t LeakCompanion::GetCurrentCount() {
    return companions_.size();
}

void LeakCompanion::AssertCount(size_t size) {
    mutex_.lock();
    auto count = GetCurrentCount();
    logger.i(TAG, "尚未释放的对象[" + std::to_string(count) + "]");
    if (size != count) {
        logger.i(TAG, "尚未释放的对象[" + std::to_string(count) + "] != 期望值[" + std::to_string(size) + "]");
        auto it = companions_.begin();
        while (it != companions_.end()) {
            logger.i(TAG, "尚未释放的对象[" + (*it)->name_ + "]");
            it++;
        }
    }
    mutex_.unlock();
}
