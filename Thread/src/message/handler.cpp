//
// Created by lixiaoqing on 2021/8/16.
//

#include "message/handler.h"

#include "message/looper.h"
#include "message/message.h"
#include "thread/thread_log.h"

#define TAG "Handler"


Handler::Handler(Looper *looper) : looper(nullptr) {
    if (looper == nullptr) {
        logger.i(TAG, "Handler 的构造函数传入的 looper 为空！");
    }
    this->looper = looper;
}

void Handler::Post(Runnable runnable) {
    auto msg = GetPostMessage(runnable);
    SendMessageDelayed(msg, 0);
}

void Handler::PostDelay(Runnable runnable, Timestamp delay) {
    auto msg = GetPostMessage(runnable);
    SendMessageDelayed(msg, delay);
}

Message *Handler::GetPostMessage(Runnable runnable) {
    auto msg = Message::Obtain();
    msg->callback_ = runnable;
    return msg;
}

void Handler::SendMessageDelayed(Message *message, Timestamp delay) {
    if (message == nullptr) {
        return;
    }
    auto now = ThreadUtil::TimeMills();
    SendMessageAtTime(message, now + delay);
}

void Handler::SendMessageAtTime(Message *message, Timestamp mills) {
    if (message == nullptr) {
        return;
    }
    message->target_ = this;
    if (this->looper == nullptr) {
        logger.i(TAG, "SendMessageAtTime 时，looper 为空！");
        return;
    }
    auto queue = this->looper->queue_;
    queue->EnqueueMessage(message, mills);
}

void Handler::DispatchMessage(Message *message) {
    if (message == nullptr) {
        return;
    }

    if (message->callback_ != nullptr) {
        message->callback_();
        message->Recycle();
        return;
    }

    if (this->callback) {
        auto ret = this->callback(message);
        if (ret) {
            message->Recycle();
            return;
        }
    }

    HandlerMessage(message);
    message->Recycle();
}

void Handler::HandlerMessage(Message *message) {

}

void Handler::SetCallback(HandlerCallback callback) {
    this->callback = callback;
}