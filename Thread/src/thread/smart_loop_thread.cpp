//
// Created by lixiaoqing on 2021/8/13.
//

#include "thread/smart_loop_thread.h"

#include "thread/thread_log.h"

#define TAG "SmartLoopThread"


SmartLoopThread::SmartLoopThread() : LoopThread() {
    logger.i(TAG, "SmartLoopThread::SmartLoopThread() called.");
}

SmartLoopThread::~SmartLoopThread() {
    logger.i(TAG, "SmartLoopThread::~SmartLoopThread() called.");
}

SmartLoopThread *SmartLoopThread::NewInstance() {
    auto thread = new SmartLoopThread();
    thread->SetOnFinishCallback([]() {});
    return thread;
}

void SmartLoopThread::SetOnFinishCallback(const Runnable &callback) {
    LoopThread::SetOnFinishCallback([this, callback]() {
        callback();
        delete this;
    });
}