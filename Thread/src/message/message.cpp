//
// Created by lixiaoqing on 2021/8/13.
//

#include "message/message.h"

#include "lock/smart_recursive_lock.h"
#include "thread/thread_log.h"

#define TAG "Message"


Message *Message::message_pool_ = nullptr;
int Message::message_pool_size_ = 0;
std::recursive_mutex Message::mutex_;


Message *Message::Obtain() {
//    logger.i(TAG, "等待 obtain()");
    SmartRecursiveLock lock(mutex_, "Message-Mutex");

    if (message_pool_ == nullptr) {
//        logger.i(TAG, "返回一个新的 Message 对象");
        auto message = new Message();
        return message;
    }

    auto msg = message_pool_;
    message_pool_ = msg->next_;
    msg->next_ = nullptr;
    msg->is_in_use_ = true;

    if (message_pool_size_ != 0) {
        --message_pool_size_;
    }
    return msg;
}

Message::Message() {
    Init();
    this->is_in_use_ = true;
}

void Message::Init() {
    this->what_ = 0;
    this->when_ = 0;
    this->arg1_ = 0;
    this->arg2_ = 0;
    this->next_ = nullptr;
    this->target_ = nullptr;
    this->callback_ = nullptr;
}

void Message::Recycle() {
//    logger.i(TAG, "Message 等待 recycle.");
    SmartRecursiveLock lock(mutex_, "Message-Mutex");
    if (!this->is_in_use_) {
        return;
    }

    // 如果回收池已经满了
    if (message_pool_size_ > kMaxPoolSize) {
        delete this;
        return;
    }

    // 加入到回收池
    Init();
    this->next_ = message_pool_;
    this->is_in_use_ = false;
    message_pool_ = this;
    ++message_pool_size_;
//    logger.i(TAG, "Message 已经被 recycle.");
}