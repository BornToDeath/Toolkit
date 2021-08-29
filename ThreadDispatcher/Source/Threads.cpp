//
// Created by lixiaoqing on 2021/8/29.
//

#include "Threads.h"
#include "ThreadDispatcher.h"


const std::string Threads::SERIAL_QUEUE = "DEFAULT_SERIAL_QUEUE";

const std::string Threads::CONCURRENT_QUEUE = "DEFAULT_CONCURRENT_QUEUE";

std::map<std::string, HandlerThread *> Threads::threads;

std::recursive_mutex Threads::nameMutex;


void Threads::runOnThread(const std::string &name, Runnable runnable, const std::string &tag) {
    // 先判断当前线程的名字，如果和 name 相同，则立即执行
    auto current = Thread::currentThread();
    if (current != nullptr && current->getName() == name) {
        runnable();
        return;
    }

    // 如果当前就在串行线程池
    if (name == SERIAL_QUEUE) {
        auto dispatcher = ThreadDispatcher::getDefaultThreadDispatcher();
        int flag = dispatcher->getCurrentThreadFlag();
        if (flag == ThreadDispatcher::THREAD_FLAG_SERIAL) {
            runnable();
            return;
        }
    }

    // 如果当前就在并发线程池
    if (name == CONCURRENT_QUEUE) {
        auto dispatcher = ThreadDispatcher::getDefaultThreadDispatcher();
        int flag = dispatcher->getCurrentThreadFlag();
        if (flag == ThreadDispatcher::THREAD_FLAG_CONCURRENT) {
            runnable();
            return;
        }
    }

    // 否则 post 到对应的线程
    postOnThread(name, runnable, 0, tag);
}

void Threads::postOnThread(const std::string &name, Runnable runnable, Timestamp delayMills, const std::string &tag) {
    if (name == SERIAL_QUEUE) {
        auto dispatcher = ThreadDispatcher::getDefaultThreadDispatcher();
        dispatcher->runOnSerial(runnable);
        nameMutex.unlock();
        return;
    }

    if (name == CONCURRENT_QUEUE) {
        auto dispatcher = ThreadDispatcher::getDefaultThreadDispatcher();
        dispatcher->runOnConcurrent(runnable);
        nameMutex.unlock();
        return;
    }

    // 上锁
    nameMutex.lock();

    // name 对应的线程
    HandlerThread *thread = nullptr;

    // 查找 name 对应的线程
    auto it = threads.find(name);
    if (it == threads.end()) {
        // 创建新线程
        thread = HandlerThread::newThread(name);
        thread->start();
        threads.insert(std::make_pair(name, thread));
    } else {
        thread = it->second;
    }

    // 解锁
    nameMutex.unlock();

    // 执行 runnable
    thread->getHandler()->postDelay(runnable, delayMills);
}

void Threads::releaseThread(const std::string &name) {
    {
        std::unique_lock<std::recursive_mutex> lock(nameMutex);
        // 查找 name 对应的线程
        auto it = threads.find(name);
        if (it != threads.end()) {
            it->second->quitThenDeleteSelf(true);
            threads.erase(it);
        }
    }
}