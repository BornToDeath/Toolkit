//
// Created by lixiaoqing on 2021/8/13.
//

#ifndef THREAD_HANDLER_H
#define THREAD_HANDLER_H


#include "Thread/Runnable.h"
#include "Thread/ThreadUtil.h"


class Looper;

class Message;

using HandlerCallback = std::function<bool(Message *)>;


class Handler {

public:

    explicit Handler(Looper *looper);

    ~Handler() = default;

    /**
     * 立即执行一个 Runnable
     * @param runnable
     */
    void post(Runnable runnable);

    /**
     * 延迟执行一个 Runnable
     * @param runnable
     * @param delay
     */
    void postDelay(Runnable runnable, Timestamp delay);

    /**
     * 延迟发送一个 Message
     * @param message
     * @param delay
     */
    void sendMessageDelayed(Message *message, Timestamp delay);

    /**
     * 在指定时间发送一个 Message
     * @param message
     * @param mills
     */
    void sendMessageAtTime(Message *message, Timestamp mills);

    void handlerMessage(Message *message);

    /**
     * 设置 Handler 的回调
     * @param callback
     */
    void setCallback(HandlerCallback callback);

    friend class Looper;

private:

    static Message *getPostMessage(Runnable runnable);

    void dispatchMessage(Message *message);

private:

    /**
     * 与此 Handler 绑定的 Looper
     */
    Looper *looper;

    /**
     * Handler 执行完消息后的回调
     */
    HandlerCallback callback;
};


#endif //THREAD_HANDLER_H
