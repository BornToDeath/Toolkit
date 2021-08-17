//
// Created by lixiaoqing on 2021/8/13.
//

#include "Thread/HandlerThread.h"
#include "Thread/ThreadLog.h"

#define TAG "HandlerThread"


HandlerThread *HandlerThread::newThread(const std::string &name) {
    auto thread = new HandlerThread(name);
    return thread;
}

HandlerThread::HandlerThread(const std::string &name) : Thread() {
    mName = name;
    isStopped = false;
    isAutoDelete = false;
    looper = nullptr;
    handler = nullptr;
    __companion.setName(name);

    auto runnable = [this]() {
        // 创建 Looper
        {
            std::unique_lock<std::mutex> lock(mutex);
            Looper::prepare();
            looper = Looper::myLooper();
            looper->setName(this->getName());
            handler = new Handler(looper);
            condition.notify_all();
        }

        // 回调子类
        onLooperPrepared();

        // 启动 loop 循环
        Looper::loop();

        // 标记线程为停止状态
        this->isStopped = true;
        onStopped();
    };

    setRunnable(runnable);
    setOnFinishCallback(nullptr);
}

HandlerThread::~HandlerThread() {
//    logger.i(TAG, "HandlerThread 线程 %s 析构", mName.c_str());
    delete handler;
    handler = nullptr;

    delete looper;
    looper = nullptr;
}

Looper *HandlerThread::getLooper() {
    if (isStopped) {
        return nullptr;
    }

    if (looper != nullptr) {
        return looper;
    }

    {
        std::unique_lock<std::mutex> lock(mutex);
        condition.wait(lock, [this] {
            return this->looper != nullptr;
        });
        return looper;
    }
}

Handler *HandlerThread::getHandler() {
    if (handler == nullptr) {
        // 保证 handler 已经创建好了
        getLooper();
    }
    return handler;
}

void HandlerThread::setOnFinishCallback(const Runnable &runnable) {
    Thread::setOnFinishCallback([this, runnable]() {
        if (runnable) {
            runnable();
        }
        if (this->isAutoDelete) {
            logger.i(TAG, "%s 线程已销毁", this->mName.c_str());
            delete this;
        }
    });
}

void HandlerThread::quit() {
    if (looper != nullptr) {
        looper->quit();
    }
}

void HandlerThread::quitSafely() {
    if (looper != nullptr) {
        looper->quitSafely();
    }
}

void HandlerThread::quitThenDeleteSelf(bool isSafely) {
    isSafely ? quitSafely() : quit();
    this->isAutoDelete = true;
}

void HandlerThread::onLooperPrepared() {

}

void HandlerThread::onStopped() {

}