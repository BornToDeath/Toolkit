//
// Created by lixiaoqing on 2021/8/12.
//

#include "Thread/LoopThread.h"
#include "Thread/ThreadLog.h"

#define TAG "LoopThread"


LoopThread::LoopThread() : Thread(), mIsRunning(false) {
    auto runnable = [this]() {
        mIsRunning = true;
        while (mIsRunning) {
            // 执行所有的任务
            runTasks();

            if (mIsRunning) {
//                logger.i("LoopThread", std::string(mName).append(" Wait Notify."));
                std::unique_lock<std::mutex> lock(mMutex);
                mTaskCondition.wait(lock);
            }
        }
//        logger.i(TAG, mName + " 线程 Loop 结束");
    };
    setRunnable(runnable);
    __companion.setName("LoopThread");
}

LoopThread::~LoopThread() {
    logger.i(TAG, std::string(mName).append(" ~LoopThread() Called"));
}

void LoopThread::runTasks() {

    // 队列不为空时，一直执行
    while (mIsRunning) {

        // 如果没有任务，退出循环
        if (mTaskQueue.empty()) {
            break;
        }

        Runnable nextTask;
        {
            // 如果有任务，出栈
            std::unique_lock<std::mutex> lock(mQueueMutex);
            nextTask = mTaskQueue.front();
            mTaskQueue.pop();
        }

        // 执行
        if (nextTask) {
            nextTask();
        }

    }
}

void LoopThread::post(const Runnable &runnable) {
    if (runnable) {
        {
            std::unique_lock<std::mutex> lock(mQueueMutex);
            mTaskQueue.push(runnable);
        }
        mTaskCondition.notify_all();
    }
}

void LoopThread::tryStop() {
//    logger.i(TAG, mName + "|LoopThread::tryStop() Called! ");
    mIsRunning = false;
    mTaskCondition.notify_all();
}