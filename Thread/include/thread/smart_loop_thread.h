//
// Created by lixiaoqing on 2021/8/13.
//

#ifndef THREAD_SMART_LOOP_THREAD_H
#define THREAD_SMART_LOOP_THREAD_H


#include "loop_thread.h"


class SmartLoopThread : public LoopThread {

public:

    static SmartLoopThread *NewInstance();

    void SetOnFinishCallback(const Runnable &callback) override;

private:

    SmartLoopThread();

    ~SmartLoopThread() override;

};


#endif //THREAD_SMART_LOOP_THREAD_H
