//
// Created by lixiaoqing on 2021/8/12.
//

#include <iostream>
#include <unistd.h>
#include <sys/prctl.h>
#include "Thread/Thread.h"
#include "Thread/ThreadLocal.h"
#include "Thread/ThreadLog.h"
#include "Thread/ThreadUtil.h"

#define TAG "Thread"

ThreadLocal<Thread> *Thread::sCurrentThread = nullptr;
std::function<void(const std::string &threadName)> Thread::onThreadStartListener;


Thread::Thread() {
    mStarted = false;
    mRunnable = nullptr;
    if (sCurrentThread == nullptr) {
        sCurrentThread = new ThreadLocal<Thread>();
    }
    __companion.setName("Thread");
}

Thread::Thread(const Runnable &runnable) {
    mStarted = false;
    mRunnable = runnable;
    if (sCurrentThread == nullptr) {
        sCurrentThread = new ThreadLocal<Thread>();
    }
    __companion.setName("Thread");
}

Thread *Thread::currentThread() {
    if (sCurrentThread == nullptr) {
//        sCurrentThread = new ThreadLocal<Thread>();
        return nullptr;
    }
    auto thread = sCurrentThread->get();
    return thread;
}

void Thread::start() {
    if (mStarted) {
        logger.i(TAG, "Started!");
        return;
    }

//    logger.i(TAG, "Thread::start() called, thread name: " + mName);
    mStarted = true;

    auto _functional = [this]() {

        // 设置线程名
        if (!mName.empty()) {
            prctl(PR_SET_NAME, mName.c_str());
        }

        logger.i(TAG, "Started!");

        // 暂存当前线程
        sCurrentThread->set(this);

        if (onThreadStartListener) {
            onThreadStartListener(mName);
        }

        if (mRunnable) {
            mRunnable();
        }

        logger.i(TAG, "Finished!");

        if (mOnFinishCallback) {
            mOnFinishCallback();
        }

        // 取消暂存当前线程
        sCurrentThread->set(nullptr);
    };

    mSTDThread = new std::thread(_functional);
    mSTDThread->detach();
}

Thread::~Thread() {
    logger.i(TAG, "Thread::~Thread() called!");
    if (mSTDThread) {
        delete mSTDThread;
        mSTDThread = nullptr;
    }
}

void Thread::setRunnable(const Runnable &runnable) {
    if (mStarted) {
        logger.i(TAG, "Thread Already Started! setRunnable invalid !");
        return;
    }
    mRunnable = runnable;
}

void Thread::setOnFinishCallback(const Runnable &runnable) {
    mOnFinishCallback = runnable;
}


void Thread::setName(std::string name) {
    __companion.setName(TAG + name);
    this->mName = name;
}

std::string Thread::getName() {
    return mName;
}

void Thread::sleep(unsigned int mills) {
    usleep(mills * 1000);
}

unsigned long long Thread::currentTimeMills() {
    return ThreadUtil::timeMills();
}