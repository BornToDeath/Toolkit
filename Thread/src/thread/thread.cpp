//
// Created by lixiaoqing on 2021/8/12.
//

#include "thread/thread.h"

#include <sys/prctl.h>
#include <unistd.h>

#include <iostream>

#include "thread/thread_local.h"
#include "thread/thread_log.h"
#include "thread/thread_util.h"

#define TAG "Thread"

ThreadLocal<Thread> *Thread::local_thread_ = nullptr;
std::function<void(const std::string &threadName)> Thread::on_thread_start_listener_;


Thread::Thread() {
    is_started_ = false;
    runnable_ = nullptr;
    if (local_thread_ == nullptr) {
        local_thread_ = new ThreadLocal<Thread>();
    }
    leak_companion_.SetName("Thread");
}

Thread::Thread(const Runnable &runnable) {
    is_started_ = false;
    runnable_ = runnable;
    if (local_thread_ == nullptr) {
        local_thread_ = new ThreadLocal<Thread>();
    }
    leak_companion_.SetName("Thread");
}

Thread::~Thread() {
    logger.i(TAG, "Thread::~Thread() called!");
    is_started_ = false;
    if (thread_) {
        delete thread_;
        thread_ = nullptr;
    }
}

Thread *Thread::CurrentThread() {
    if (local_thread_) {
        return local_thread_->Get();
    }
    return nullptr;
}

void Thread::Start() {
    if (is_started_) {
//        logger.i(TAG, "Started!");
        return;
    }

//    logger.i(TAG, "Thread::start() called, thread name: " + mName);
    is_started_ = true;

    auto _functional = [this]() {

        // 设置线程名
        if (!name_.empty()) {
            prctl(PR_SET_NAME, name_.c_str());
        }

        logger.i(TAG, "%s Started!", name_.c_str());

        // 暂存当前线程
        if (local_thread_ == nullptr) {
            logger.i(TAG, "local thread is nullptr!");
            return;
        }
        local_thread_->Set(this);

        if (on_thread_start_listener_) {
            on_thread_start_listener_(name_);
        }

        if (runnable_) {
            runnable_();
        }

        logger.i(TAG, "Finished!");

        if (on_finish_callback_) {
            on_finish_callback_();
        }

        // 取消暂存当前线程
        local_thread_->Set(nullptr);
    };

    thread_ = new std::thread(_functional);
    thread_->detach();
}

void Thread::SetRunnable(const Runnable &runnable) {
    if (is_started_) {
        logger.i(TAG, "Thread Already Started! SetRunnable invalid !");
        return;
    }
    runnable_ = runnable;
}

void Thread::SetOnFinishCallback(const Runnable &runnable) {
    on_finish_callback_ = runnable;
}

void Thread::SetName(const std::string &name) {
    this->name_ = name;
    leak_companion_.SetName(TAG + name);
}

std::string Thread::GetName() {
    return name_;
}

void Thread::Sleep(unsigned int mills) {
    usleep(mills * 1000);
}

unsigned long long Thread::CurrentTimeMills() {
    return ThreadUtil::TimeMills();
}