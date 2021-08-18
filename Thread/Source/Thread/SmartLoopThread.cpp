//
// Created by lixiaoqing on 2021/8/13.
//

#include "Thread/SmartLoopThread.h"
#include "Thread/ThreadLog.h"

#define TAG "SmartLoopThread"


SmartLoopThread::SmartLoopThread() : LoopThread() {
    logger.i(TAG, "SmartLoopThread::SmartLoopThread() called.");
}

SmartLoopThread::~SmartLoopThread() {
    logger.i(TAG, "SmartLoopThread::~SmartLoopThread() called.");
}

SmartLoopThread *SmartLoopThread::newThread() {
    auto thread = new SmartLoopThread();
    thread->setOnFinishCallback([]() {});
    return thread;
}

void SmartLoopThread::setOnFinishCallback(const Runnable &callback) {
    LoopThread::setOnFinishCallback([this, callback]() {
        callback();
        delete this;
    });
}