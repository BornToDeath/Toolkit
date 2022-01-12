//
// Created by lixiaoqing on 2021/8/13.
//

#include "Looper/Looper.h"
#include "Thread/ThreadUtil.h"
#include "Thread/ThreadLog.h"

#define TAG "Looper"


ThreadLocal<Looper> *Looper::threadLocal = nullptr;


Looper::Looper() {
    queue = new MessageQueue();
    thread = nullptr;
}

Looper::~Looper() {
    delete queue;
}

Looper *Looper::myLooper() {
    return threadLocal->get();
}

bool Looper::prepare() {
    // 这个方法首次调用时，不可能在多个线程。所以一定是线程安全的
    logger.i(TAG, "Looper preparing.");
    if (threadLocal == nullptr) {
        threadLocal = new ThreadLocal<Looper>();
    }

    auto looper = threadLocal->get();
    if (looper != nullptr) {
        return true;
    }

    auto thread = Thread::currentThread();
    if (thread == nullptr) {
        return false;
    }

    looper = new Looper();
    looper->thread = thread;
    threadLocal->set(looper);
    return true;
}

void Looper::loop() {
    auto looper = Looper::myLooper();
    if (looper == nullptr) {
        return;
    }

    logger.i(TAG, "启动 looper 循环");
    while (true) {
        // 取一条消息
        auto message = looper->queue->next();
        if (message == nullptr) {
            logger.i(TAG, "退出 Looper 循环");
            return;
        }
        if (message->target) {
            auto t1 = ThreadUtil::timeMills();
            // 处理消息
            message->target->dispatchMessage(message);
            auto t2 = ThreadUtil::timeMills();
            logger.i(TAG, "Message 执行耗时: %llu 毫秒", (t2 - t1));
        }
    }
}

void Looper::quit() {
    queue->quit(false);
}

void Looper::quitSafely() {
    queue->quit(false);
}

void Looper::setName(const std::string &name) {
    this->name = name;
    this->queue->setName(name);
}