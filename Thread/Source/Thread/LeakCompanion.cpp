//
// Created by lixiaoqing on 2021/8/12.
//

#include "Thread/LeakCompanion.h"
#include "Thread/ThreadLog.h"

#define TAG "LeakCompanion"

std::vector<LeakCompanion *> LeakCompanion::all;
std::recursive_mutex LeakCompanion::mutex;


LeakCompanion::LeakCompanion() {
    name = "Default";
//    logger.i(TAG, "LeakCompanion ++, name=" + name);

    mutex.lock();
    all.push_back(this);
    mutex.unlock();
}

LeakCompanion::LeakCompanion(std::string name) {
    this->name = std::move(name);
//    logger.i(TAG, "LeakCompanion ++, name=" + name);

    mutex.lock();
    all.push_back(this);
    mutex.unlock();
}

LeakCompanion::~LeakCompanion() {
//    logger.i(TAG, "LeakCompanion --, name=" + name);
    mutex.lock();
    auto it = all.begin();
    while (it != all.end()) {
        if (*it == this) {
            all.erase(it);
            break;
        }
        it++;
    }
    mutex.unlock();
}

void LeakCompanion::setName(std::string name_) {
    this->name = std::move(name_);
}

size_t LeakCompanion::getCurrentCount() {
    return all.size();
}

void LeakCompanion::assertCount(size_t size) {
    mutex.lock();
    auto count = getCurrentCount();
    logger.i(TAG, "尚未释放的对象[" + std::to_string(count) + "]");
    if (size != count) {
        logger.i(TAG, "尚未释放的对象[" + std::to_string(count) + "] != 期望值[" + std::to_string(size) + "]");
        auto it = all.begin();
        while (it != all.end()) {
            logger.i(TAG, "尚未释放的对象[" + (*it)->name + "]");
            it++;
        }
    }
    mutex.unlock();
}
