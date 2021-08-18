//
// Created by lixiaoqing on 2021/8/12.
//

#include "Thread/SmartThread.h"
#include "Thread/ThreadLog.h"

#define TAG "SmartThread"


SmartThread *SmartThread::newThread(Runnable runnable) {
    auto *thread = new SmartThread(std::move(runnable));

    // 设置结束时的回调（释放自己）
    thread->setOnFinishCallback([]() {});
    return thread;
}

SmartThread::SmartThread(Runnable runnable) : Thread(runnable) {
    __companion.setName("SmartThread");
}

SmartThread::~SmartThread() {
    logger.i(TAG, "~SmartThread() called!");
}

void SmartThread::setName(std::string name) {
    Thread::setName(name);
    __companion.setName("SmartThread_" + name);
}

void SmartThread::setOnFinishCallback(const Runnable &runnable) {
    Thread::setOnFinishCallback([this, runnable]() {
        if (runnable) {
            runnable();
        }
        delete this;
    });
}