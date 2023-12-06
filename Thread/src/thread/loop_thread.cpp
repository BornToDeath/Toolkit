//
// Created by lixiaoqing on 2021/8/12.
//

#include "thread/loop_thread.h"

#include "thread/thread_log.h"

#define TAG "LoopThread"


LoopThread::LoopThread() : Thread(), is_running_(false) {
    auto runnable = [this]() {
        is_running_ = true;
        while (is_running_) {
            // 执行所有的任务
            RunTasks();

            if (is_running_) {
//                logger.i("LoopThread", std::string(mName).append(" Wait Notify."));
                std::unique_lock<std::mutex> lock(mutex_);
                task_condition_.wait(lock);
            }
        }
//        logger.i(TAG, mName + " 线程 Loop 结束");
    };
    SetRunnable(runnable);
    companion_.SetName("LoopThread");
}

LoopThread::~LoopThread() {
    logger.i(TAG, std::string(name_).append(" ~LoopThread() called."));
}

void LoopThread::RunTasks() {

    // 队列不为空时，一直执行
    while (is_running_) {

        // 如果没有任务，退出循环
        if (task_queue_.empty()) {
            break;
        }

        Runnable next_task;
        {
            // 如果有任务，出队列
            std::unique_lock<std::mutex> lock(queue_mutex_);
            next_task = task_queue_.front();
            task_queue_.pop();
        }

        // 执行
        if (next_task) {
            next_task();
        }

    }
}

void LoopThread::Post(const Runnable &runnable) {
    if (runnable) {
        {
            std::unique_lock<std::mutex> lock(queue_mutex_);
            task_queue_.push(runnable);
        }
        task_condition_.notify_all();
    }
}

void LoopThread::TryStop() {
//    logger.i(TAG, mName + "|LoopThread::tryStop() Called! ");
    is_running_ = false;
    task_condition_.notify_all();
}