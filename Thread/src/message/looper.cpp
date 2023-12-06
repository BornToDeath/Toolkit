//
// Created by lixiaoqing on 2021/8/13.
//

#include "message/looper.h"

#include "thread/thread_log.h"
#include "thread/thread_util.h"

#define TAG "Looper"


ThreadLocal<Looper> *Looper::thread_local_ = nullptr;


Looper::Looper() {
    queue_ = new MessageQueue();
    thread_ = nullptr;
}

Looper::~Looper() {
    delete queue_;
    queue_ = nullptr;
}

Looper *Looper::MyLooper() {
    if (thread_local_ == nullptr) {
        logger.i(TAG, "MyLooper failed, thread local is nullptr");
        return nullptr;
    }
    return thread_local_->Get();
}

bool Looper::Prepare() {
    // 这个方法首次调用时，不可能在多个线程。所以一定是线程安全的
    logger.i(TAG, "Looper preparing.");
    if (thread_local_ == nullptr) {
        thread_local_ = new ThreadLocal<Looper>();
    }

    auto looper = thread_local_->Get();
    if (looper != nullptr) {
        return true;
    }

    auto thread = Thread::CurrentThread();
    if (thread == nullptr) {
        return false;
    }

    looper = new Looper();
    looper->thread_ = thread;
    thread_local_->Set(looper);
    return true;
}

void Looper::Loop() {
    auto looper = Looper::MyLooper();
    if (looper == nullptr) {
        return;
    }

    logger.i(TAG, "启动 looper 循环");
    while (true) {
        // 取一条消息
        auto message = looper->queue_->Next();
        if (message == nullptr) {
            logger.i(TAG, "退出 Looper 循环");
            return;
        }
        if (message->target_) {
            auto t1 = ThreadUtil::TimeMills();
            // 处理消息
            message->target_->DispatchMessage(message);
            auto t2 = ThreadUtil::TimeMills();
            logger.i(TAG, "Message 执行耗时: %llu 毫秒", (t2 - t1));
        }
    }
}

void Looper::Quit() const {
    queue_->Quit(false);
}

void Looper::QuitSafely() const {
    queue_->Quit(false);
}

void Looper::SetName(const std::string &name) {
    this->name_ = name;
    this->queue_->SetName(name);
}