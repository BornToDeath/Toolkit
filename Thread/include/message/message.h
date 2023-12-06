//
// Created by lixiaoqing on 2021/8/13.
//

#ifndef THREAD_MESSAGE_H
#define THREAD_MESSAGE_H


#include <mutex>

#include "handler.h"
#include "thread/runnable.h"

using Timestamp = unsigned long long;

class Message {
private:
    Message();

public:

    /**
     * 获取一个可用消息
     * @return
     */
    static Message *Obtain();

    /**
     * 回收消息。将消息放回回收池中
     */
    void Recycle();

private:
    void Init();

public:

    Handler *target_{nullptr};

    Runnable callback_;

    Timestamp when_{0};

    Message *next_{nullptr};

    int what_{0};

    int arg1_{0};

    int arg2_{0};

private:

    /**
     * 标记当前消息是否可用
     */
    bool is_in_use_;

    /**
     * 消息回收池
     */
    static Message *message_pool_;

    /**
     * 回收池的大小
     */
    static int message_pool_size_;

    /**
     * 回收池的容量
     */
    static const int kMaxPoolSize = 50;

    static std::recursive_mutex mutex_;

};


#endif //THREAD_MESSAGE_H
