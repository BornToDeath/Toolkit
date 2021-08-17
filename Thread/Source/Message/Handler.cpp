//
// Created by lixiaoqing on 2021/8/16.
//

#include "Handler/Handler.h"
#include "Message/Message.h"
#include "Thread/ThreadLog.h"
#include "Looper/Looper.h"

#define TAG "Handler"


Handler::Handler(Looper *looper) : looper(nullptr) {
    if (looper == nullptr) {
        logger.i(TAG, "Handler 的构造函数传入的 looper 为空！");
    }
    this->looper = looper;
}

void Handler::post(Runnable runnable) {
    auto msg = getPostMessage(runnable);
    sendMessageDelayed(msg, 0);
}

void Handler::postDelay(Runnable runnable, Timestamp delay) {
    auto msg = getPostMessage(runnable);
    sendMessageDelayed(msg, delay);
}

Message *Handler::getPostMessage(Runnable runnable) {
    auto msg = Message::obtain();
    msg->callback = runnable;
    return msg;
}

void Handler::sendMessageDelayed(Message *message, Timestamp delay) {
    if (message == nullptr) {
        return;
    }
    auto now = ThreadUtil::timeMills();
    sendMessageAtTime(message, now + delay);
}

void Handler::sendMessageAtTime(Message *message, Timestamp mills) {
    if (message == nullptr) {
        return;
    }
    message->target = this;
    if (this->looper == nullptr) {
        logger.i(TAG, "sendMessageAtTime 时，looper 为空！");
        return;
    }
    auto queue = this->looper->queue;
    queue->enqueueMessage(message, mills);
}

void Handler::dispatchMessage(Message *message) {
    if (message == nullptr) {
        return;
    }

    if (message->callback != nullptr) {
        message->callback();
        message->recycle();
        return;
    }

    if (this->callback) {
        auto ret = this->callback(message);
        if (ret) {
            message->recycle();
            return;
        }
    }

    handlerMessage(message);
    message->recycle();
}

void Handler::handlerMessage(Message *message) {

}

void Handler::setCallback(HandlerCallback callback) {
    this->callback = callback;
}