//
// Created by lixiaoqing on 2021/8/12.
//

#ifndef THREAD_LOOP_THREAD_H
#define THREAD_LOOP_THREAD_H


#include <condition_variable>
#include <queue>
#include "thread.h"


class LoopThread : public Thread {

public:

    LoopThread();

    ~LoopThread() override;

public:

//    void setRunnable(const Runnable &runnable) override;

    virtual void Post(const Runnable & runnable);

    virtual void TryStop();

private:

    void RunTasks();

private:

    ADD_LEAK_DETECTOR

    bool is_running_;

    std::mutex mutex_;

    std::condition_variable task_condition_;

    std::queue<Runnable> task_queue_;

    std::mutex queue_mutex_;
};


#endif //THREAD_LOOP_THREAD_H
