//
// Created by lixiaoqing on 2021/8/19.
//

#include "ThreadDispatcher.h"
#include "Thread/ThreadLog.h"

#define TAG "ThreadDispatcher"


ThreadDispatcher *ThreadDispatcher::defaultDispatcher = nullptr;
static std::mutex mutex;


ThreadDispatcher::ThreadDispatcher(const std::string &name, size_t maxConcurrentThreadNum) {
    busThread = HandlerThread::newThread(name + "-Bus");
    busThread->start();
    busThread->getHandler()->post([this] {
        threadFlags.set(new int(THREAD_FLAG_BUS));
    });

    serialPool = ThreadPool::newThreadPool(name + "-Serial", 1, [this] {
        threadFlags.set(new int(THREAD_FLAG_SERIAL));
    });

    concurrentPool = ThreadPool::newThreadPool(name + "-Concurt", maxConcurrentThreadNum, [this]() {
        threadFlags.set(new int(THREAD_FLAG_CONCURRENT));
    });

    logger.i(TAG, "构造方法执行完毕！");
}

ThreadDispatcher::~ThreadDispatcher() {
    busThread->quitThenDeleteSelf(true);
    serialPool->shutDownAndDeleteSelf();
    concurrentPool->shutDownAndDeleteSelf();
}

void ThreadDispatcher::runOnBus(Runnable runnable) {
    auto handler = busThread->getHandler();
    if (handler) {
        handler->post(runnable);
    }
}

void ThreadDispatcher::postOnBus(Runnable runnable, Timestamp delay) {
    auto handler = busThread->getHandler();
    if (handler) {
        handler->postDelay(runnable, delay);
    }
}

void ThreadDispatcher::runOnSerial(Runnable runnable, std::string name) {
    serialPool->execute(name, runnable);
}

void ThreadDispatcher::runOnConcurrent(Runnable runnable, std::string name) {
    concurrentPool->execute(name, runnable);
}

int ThreadDispatcher::getCurrentThreadFlag() {
    auto flag = threadFlags.get();
    if (flag == nullptr) {
        return 0;
    }
    return *flag;
}

ThreadDispatcher *ThreadDispatcher::getDefaultThreadDispatcher() {
    if (defaultDispatcher == nullptr) {
        std::unique_lock<std::mutex> lock(mutex);
        if (defaultDispatcher == nullptr) {
            defaultDispatcher = new ThreadDispatcher("DEF", 4);
        }
    }
    return defaultDispatcher;
}