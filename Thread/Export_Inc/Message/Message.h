//
// Created by lixiaoqing on 2021/8/13.
//

#ifndef THREAD_MESSAGE_H
#define THREAD_MESSAGE_H


#include <mutex>
#include "Thread/Runnable.h"
#include "Handler/Handler.h"

using Timestamp = unsigned long long;


class Message {

public:

    /**
     * 获取一个可用消息
     * @return
     */
    static Message *obtain();

    /**
     * 回收消息。将消息放回回收池中
     */
    void recycle();

private:

    Message();

    void init();

public:

    Message *next;

    Handler *target;

    Runnable callback;

    Timestamp when;

    int what;

    int arg1;

    int arg2;

private:

    /**
     * 标记当前消息是否可用
     */
    bool isInUse;

    /**
     * 消息回收池
     */
    static Message *sPool;

    /**
     * 回收池的大小
     */
    static int sPoolSize;

    /**
     * 回收池的容量
     */
    static const int MAX_POOL_SIZE = 50;

    static std::recursive_mutex mutex;

};


#endif //THREAD_MESSAGE_H
