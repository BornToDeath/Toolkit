//
// Created by lixiaoqing on 2021/8/12.
//

#ifndef THREAD_LOOPTHREAD_H
#define THREAD_LOOPTHREAD_H


#include <condition_variable>
#include <queue>
#include "Thread.h"


class LoopThread : public Thread {

public:

    LoopThread();

    ~LoopThread() override;

public:

//    void setRunnable(const Runnable &runnable) override;

    virtual void post(const Runnable & runnable);

    virtual void tryStop();

private:

    void runTasks();

private:

    ADD_LEAK_DETECTOR

    bool mIsRunning;

    std::mutex mMutex;

    std::condition_variable mTaskCondition;

    std::queue<Runnable> mTaskQueue;

    std::mutex mQueueMutex;
};


#endif //THREAD_LOOPTHREAD_H
