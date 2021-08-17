//
// Created by lixiaoqing on 2021/8/13.
//

#include "Message/Message.h"
#include "Thread/ThreadLog.h"
#include "Lock/SmartRecursiveLock.h"

#define TAG "Message"


Message *Message::sPool = nullptr;
int Message::sPoolSize = 0;
std::recursive_mutex Message::mutex;


Message *Message::obtain() {
//    logger.i(TAG, "等待 obtain()");
    SmartRecursiveLock lock(mutex, "Message-Mutex");

    if (sPool == nullptr) {
//        logger.i(TAG, "返回一个新的 Message 对象");
        auto message = new Message();
        return message;
    }

    auto msg = sPool;
    sPool = msg->next;
    msg->next = nullptr;
    msg->isInUse = true;

    if (sPoolSize != 0) {
        --sPoolSize;
    }
    return msg;
}

Message::Message() {
    init();
    this->isInUse = true;
}

void Message::init() {
    this->what = 0;
    this->when = 0;
    this->arg1 = 0;
    this->arg2 = 0;
    this->next = nullptr;
    this->target = nullptr;
    this->callback = nullptr;
}

void Message::recycle() {
//    logger.i(TAG, "Message 等待 recycle.");
    SmartRecursiveLock lock(mutex, "Message-Mutex");
    if (!this->isInUse) {
        return;
    }

    // 如果回收池已经满了
    if (sPoolSize > MAX_POOL_SIZE) {
        delete this;
        return;
    }

    // 加入到回收池
    init();
    this->next = sPool;
    this->isInUse = false;
    sPool = this;
    ++sPoolSize;
//    logger.i(TAG, "Message 已经被 recycle.");
}