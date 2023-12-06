//
// Created by lixiaoqing on 2021/8/13.
//

#ifndef THREAD_LOOPER_H
#define THREAD_LOOPER_H


#include "message_queue.h"
#include "thread/thread.h"
#include "thread/thread_local.h"


class Looper {

public:

    ~Looper();

    /**
     * 获取当前线程的 looper 对象
     * @return
     */
    static Looper *MyLooper();

    /**
     * 准备当前线程的 looper 对象
     * @return
     */
    static bool Prepare();

    /**
     * 开启当前线程的 Loop 循环
     */
    static void Loop();

    void Quit() const;

    void QuitSafely() const;

    void SetName(const std::string &name);

private:

    Looper();

public:

    /**
     * 消息队列
     */
    MessageQueue *queue_;

    /**
     * Handler 类是当前类的友元类，表明 Handler 可以访问当前类的私有属性
     */
    friend class Handler;

private:

    static ThreadLocal<Looper> *thread_local_;

    Thread *thread_;

    std::string name_;

};


#endif //THREAD_LOOPER_H
