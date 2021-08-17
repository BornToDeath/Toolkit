//
// Created by lixiaoqing on 2021/8/13.
//

#ifndef THREAD_LOOPER_H
#define THREAD_LOOPER_H


#include "Message/MessageQueue.h"
#include "Thread/Thread.h"
#include "Thread/ThreadLocal.h"


class Looper {

public:

    ~Looper();

    /**
     * 获取当前线程的 looper 对象
     * @return
     */
    static Looper *myLooper();

    /**
     * 准备当前线程的 looper 对象
     * @return
     */
    static bool prepare();

    /**
     * 开启当前线程的 loop 循环
     */
    static void loop();

    void quit();

    void quitSafely();

    void setName(const std::string &name);

private:

    Looper();

public:

    /**
     * 消息队列
     */
    MessageQueue *queue;

    /**
     * Handler 类是当前类的友元类，表明 Handler 可以访问当前类的私有属性
     */
    friend class Handler;

private:

    static ThreadLocal<Looper> *threadLocal;

    Thread *thread;

    std::string name;

};


#endif //THREAD_LOOPER_H
