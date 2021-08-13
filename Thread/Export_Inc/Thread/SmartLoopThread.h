//
// Created by lixiaoqing on 2021/8/13.
//

#ifndef THREAD_SMARTLOOPTHREAD_H
#define THREAD_SMARTLOOPTHREAD_H


#include "LoopThread.h"


class SmartLoopThread : public LoopThread {

public:

    static SmartLoopThread *newThread();

    void setOnFinishCallback(const Runnable &callback) override;

private:

    SmartLoopThread();

    ~SmartLoopThread() override;

};


#endif //THREAD_SMARTLOOPTHREAD_H
