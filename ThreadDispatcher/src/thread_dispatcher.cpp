//
// Created by lixiaoqing on 2021/8/19.
//

#include "thread_dispatcher.h"

#include "thread/thread_log.h"

#define TAG "ThreadDispatcher"


ThreadDispatcher *ThreadDispatcher::default_dispatcher_ = nullptr;
static std::mutex mutex;


ThreadDispatcher::ThreadDispatcher(const std::string &name, size_t max_concurrent_thread_count) {
    bus_thread_ = HandlerThread::NewInstance(name + "-Bus");
    bus_thread_->Start();
    bus_thread_->GetHandler()->Post([this] {
        thread_flags_.Set(new int(kThreadFlagBus));
    });

    serial_pool_ = ThreadPool::NewInstance(name + "-Serial", 1, [this] {
        thread_flags_.Set(new int(kThreadFlagSerial));
    });

    concurrent_pool_ = ThreadPool::NewInstance(name + "-Concurt", max_concurrent_thread_count, [this]() {
        thread_flags_.Set(new int(kThreadFlagConcurrent));
    });

    logger.i(TAG, "构造方法执行完毕！");
}

ThreadDispatcher::~ThreadDispatcher() {
    bus_thread_->QuitThenDeleteSelf(true);
    serial_pool_->ShutDownAndDeleteSelf();
    concurrent_pool_->ShutDownAndDeleteSelf();
}

void ThreadDispatcher::RunOnBus(Runnable runnable) {
    auto handler = bus_thread_->GetHandler();
    if (handler) {
        handler->Post(runnable);
    }
}

void ThreadDispatcher::PostOnBus(Runnable runnable, Timestamp delay) {
    auto handler = bus_thread_->GetHandler();
    if (handler) {
        handler->PostDelay(runnable, delay);
    }
}

void ThreadDispatcher::RunOnSerial(Runnable runnable, std::string name) {
    serial_pool_->Execute(name, runnable);
}

void ThreadDispatcher::RunOnConcurrent(Runnable runnable, std::string name) {
    concurrent_pool_->Execute(name, runnable);
}

int ThreadDispatcher::GetCurrentThreadFlag() {
    auto flag = thread_flags_.Get();
    if (flag == nullptr) {
        return 0;
    }
    return *flag;
}

ThreadDispatcher *ThreadDispatcher::GetDefaultThreadDispatcher() {
    if (default_dispatcher_ == nullptr) {
        std::unique_lock<std::mutex> lock(mutex);
        if (default_dispatcher_ == nullptr) {
            default_dispatcher_ = new ThreadDispatcher("DEF", 4);
        }
    }
    return default_dispatcher_;
}