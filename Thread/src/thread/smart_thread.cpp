//
// Created by lixiaoqing on 2021/8/12.
//

#include "thread/smart_thread.h"

#include "thread/thread_log.h"

#define TAG "SmartThread"


SmartThread *SmartThread::NewInstance(Runnable runnable) {
    auto *thread = new SmartThread(std::move(runnable));

    // 设置结束时的回调（释放自己）
    thread->SetOnFinishCallback([]() {});
    return thread;
}

SmartThread::SmartThread(Runnable runnable) : Thread(runnable) {
    companion_.SetName("SmartThread");
}

SmartThread::~SmartThread() {
    logger.i(TAG, "~SmartThread() called!");
}

void SmartThread::SetName(const std::string &name) {
    Thread::SetName(name);
    companion_.SetName("SmartThread_" + name);
}

void SmartThread::SetOnFinishCallback(const Runnable &runnable) {
    Thread::SetOnFinishCallback([this, runnable]() {
        if (runnable) {
            runnable();
        }
        delete this;
    });
}