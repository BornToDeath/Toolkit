//
// Created by lixiaoqing on 2021/8/12.
//

#ifndef THREAD_SMARTTHREAD_H
#define THREAD_SMARTTHREAD_H


#include "Thread.h"


/**
 * 执行完后自动释放自己的线程。无法主动 delete。
 *
 * 该类实例的指针，没有被持有的必要。
 */
class SmartThread : public Thread {

public:

    /**
     * 创建对象的唯一方式
     */
    static SmartThread *newThread(Runnable runnable);

public:

    void setOnFinishCallback(const Runnable &runnable) override;

    void setName(std::string name) override;

private:

    /**
     * 私有化构造函数，外界不能通过 new 的方式在堆中创建对象。
     * 也不能通过 SmartThread thread; 的方式在栈中创建对象。
     *
     * 唯一创建对象的方式是 SmartThread::newThread();
     */
    explicit SmartThread(Runnable runnable);

    /**
     * 私有化析构函数，外界不能/不需要调用 delete 析构。
     *
     * 线程执行完后会自己释放自己的内存。
     */
    ~SmartThread() override;

    ADD_LEAK_DETECTOR
};


#endif //THREAD_SMARTTHREAD_H
