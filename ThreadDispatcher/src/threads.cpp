//
// Created by lixiaoqing on 2021/8/29.
//

#include "threads.h"

#include "thread_dispatcher.h"


const std::string Threads::kSerialQueue = "DEFAULT_SERIAL_QUEUE";

const std::string Threads::kConcurrentQueue = "DEFAULT_CONCURRENT_QUEUE";

std::map<std::string, HandlerThread *> Threads::threads_;

std::recursive_mutex Threads::threads_mutex_;


void Threads::RunOnThread(const std::string &name, Runnable runnable, const std::string &tag) {
    // 先判断当前线程的名字，如果和 name 相同，则立即执行
    auto current = Thread::CurrentThread();
    if (current != nullptr && current->GetName() == name) {
        runnable();
        return;
    }

    // 如果当前就在串行线程池
    if (name == kSerialQueue) {
        auto dispatcher = ThreadDispatcher::GetDefaultThreadDispatcher();
        int flag = dispatcher->GetCurrentThreadFlag();
        if (flag == ThreadDispatcher::kThreadFlagSerial) {
            runnable();
            return;
        }
    }

    // 如果当前就在并发线程池
    if (name == kConcurrentQueue) {
        auto dispatcher = ThreadDispatcher::GetDefaultThreadDispatcher();
        int flag = dispatcher->GetCurrentThreadFlag();
        if (flag == ThreadDispatcher::kThreadFlagConcurrent) {
            runnable();
            return;
        }
    }

    // 否则 post 到对应的线程
    PostOnThread(name, runnable, 0, tag);
}

void Threads::PostOnThread(const std::string &name, Runnable runnable, Timestamp delay_mills, const std::string &tag) {
    if (name == kSerialQueue) {
        auto dispatcher = ThreadDispatcher::GetDefaultThreadDispatcher();
        dispatcher->RunOnSerial(runnable);
        threads_mutex_.unlock();
        return;
    }

    if (name == kConcurrentQueue) {
        auto dispatcher = ThreadDispatcher::GetDefaultThreadDispatcher();
        dispatcher->RunOnConcurrent(runnable);
        threads_mutex_.unlock();
        return;
    }

    // 上锁
    threads_mutex_.lock();

    // name 对应的线程
    HandlerThread *thread = nullptr;

    // 查找 name 对应的线程
    auto it = threads_.find(name);
    if (it == threads_.end()) {
        // 创建新线程
        thread = HandlerThread::NewInstance(name);
        thread->Start();
        threads_.insert(std::make_pair(name, thread));
    } else {
        thread = it->second;
    }

    // 解锁
    threads_mutex_.unlock();

    // 执行 runnable
    thread->GetHandler()->PostDelay(runnable, delay_mills);
}

void Threads::ReleaseThread(const std::string &name) {
    {
        std::unique_lock<std::recursive_mutex> lock(threads_mutex_);
        // 查找 name 对应的线程
        auto it = threads_.find(name);
        if (it != threads_.end()) {
            it->second->QuitThenDeleteSelf(true);
            threads_.erase(it);
        }
    }
}