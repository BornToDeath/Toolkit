//
// Created by lixiaoqing on 2021/8/13.
//

#ifndef THREAD_HANDLER_H
#define THREAD_HANDLER_H


#include "thread/runnable.h"
#include "thread/thread_util.h"


class Looper;
class Message;


class Handler {
    using HandlerCallback = std::function<bool(Message *)>;

public:

    explicit Handler(Looper *looper);

    ~Handler() = default;

    /**
     * 立即执行一个 Runnable
     * @param runnable
     */
    void Post(Runnable runnable);

    /**
     * 延迟执行一个 Runnable
     * @param runnable
     * @param delay
     */
    void PostDelay(Runnable runnable, Timestamp delay);

    /**
     * 延迟发送一个 Message
     * @param message
     * @param delay
     */
    void SendMessageDelayed(Message *message, Timestamp delay);

    /**
     * 在指定时间发送一个 Message
     * @param message
     * @param mills
     */
    void SendMessageAtTime(Message *message, Timestamp mills);

    void HandlerMessage(Message *message);

    /**
     * 设置 Handler 的回调
     * @param callback
     */
    void SetCallback(HandlerCallback callback);

    friend class Looper;

private:

    static Message *GetPostMessage(Runnable runnable);

    void DispatchMessage(Message *message);

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
