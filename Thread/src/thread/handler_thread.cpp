//
// Created by lixiaoqing on 2021/8/13.
//

#include "thread/handler_thread.h"

#include "thread/thread_log.h"

#define TAG "HandlerThread"


HandlerThread *HandlerThread::NewInstance(const std::string &name) {
    auto thread = new HandlerThread(name);
    return thread;
}

HandlerThread::HandlerThread(const std::string &name) : Thread() {
    name_ = name;
    is_stopped_ = false;
    auto_delete_ = false;
    looper_ = nullptr;
    handler_ = nullptr;
    leak_companion_.SetName(name);

    auto runnable = [this]() {
        // 创建 Looper
        {
            std::unique_lock<std::mutex> lock(mutex_);
            Looper::Prepare();
            looper_ = Looper::MyLooper();
            looper_->SetName(this->GetName());
            handler_ = new Handler(looper_);
            condition_.notify_all();
        }

        // 回调子类
        OnLooperPrepared();

        // 启动 loop 循环
        Looper::Loop();

        // 标记线程为停止状态
        this->is_stopped_ = true;
        OnStopped();
    };

    SetRunnable(runnable);
    SetOnFinishCallback(nullptr);
}

HandlerThread::~HandlerThread() {
//    logger.i(TAG, "HandlerThread 线程 %s 析构", mName.c_str());
    delete handler_;
    handler_ = nullptr;

    delete looper_;
    looper_ = nullptr;
}

Looper *HandlerThread::GetLooper() {
    if (is_stopped_) {
        return nullptr;
    }

    {
        std::unique_lock<std::mutex> lock(mutex_);
        condition_.wait(lock, [this] {
            return this->looper_ != nullptr;
        });
        return looper_;
    }
}

Handler *HandlerThread::GetHandler() {
    // 保证 handler 已经创建好了
    GetLooper();
    return handler_;
}

void HandlerThread::SetOnFinishCallback(const Runnable &runnable) {
    Thread::SetOnFinishCallback([this, runnable]() {
        if (runnable) {
            runnable();
        }
        if (this->auto_delete_) {
            logger.i(TAG, "%s 线程已销毁", this->name_.c_str());
            delete this;
        }
    });
}

void HandlerThread::Quit() {
    if (looper_) {
        looper_->Quit();
    }
}

void HandlerThread::QuitSafely() {
    if (looper_) {
        looper_->QuitSafely();
    }
}

void HandlerThread::QuitThenDeleteSelf(bool is_safely) {
    is_safely ? QuitSafely() : Quit();
    this->auto_delete_ = true;
}

void HandlerThread::OnLooperPrepared() {

}

void HandlerThread::OnStopped() {

}