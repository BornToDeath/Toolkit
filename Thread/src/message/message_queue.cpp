//
// Created by lixiaoqing on 2021/8/13.
//

#include "message/message_queue.h"

#include "lock/smart_recursive_lock.h"
#include "thread/thread_log.h"
#include "thread/thread_util.h"

#define TAG "MessageQueue"


MessageQueue::MessageQueue() : is_quited_(false), has_message_(false), header_(nullptr) {

}

MessageQueue::~MessageQueue() {
//    logger.i(TAG, "~MessageQueue() called.");
}

void MessageQueue::EnqueueMessage(Message *message, Timestamp when) {
    if (is_quited_ || message == nullptr) {
        return;
    }

    message->when_ = when;

    {
//        logger.i(TAG, "enqueue a message.");

        // 保证头结点的线程安全
        SmartRecursiveLock lock(header_mutex_);

        auto p = header_;
        if (p == nullptr || when == 0 || when < p->when_) {
            message->next_ = p;
            header_ = message;
        } else {
            // 找到要插入的位置
            Message *prev;
            do {
                prev = p;
                p = p->next_;
                if (p == nullptr || when < p->when_) {
                    break;
                }
            } while (true);
            // 插入到 prev 和 p 之间
            message->next_ = p;
            prev->next_ = message;
        }
    }

    // 尝试通知一下
    Awake();
}

void MessageQueue::Awake() {
//    logger.i(TAG, "awake");
    std::unique_lock<std::mutex> lock(poll_once_mutex_);
    has_message_ = true;
    condition_.notify_all();
}

Message *MessageQueue::Next() {
    Timestamp wait_mills = 0;

    while (true) {

        /*
         * 如果在 next 的 unlock 之后，wait 之前，另一个线程调用了 EnqueueMessage，
         * 并执行了 awake，会导致这时并没有 wait 的线程，awake 了个寂寞。
         * 等到 wait 执行之后，后续再也没有线程去唤醒它了。
         *
         *
         *                 enqueue             .          next
         *                    |                .            |
         *                    v                .            | <-------------------------------+
         *                lock header          .            v                                 |
         *                    |                .          pollOnce                            |
         *                    v                .            |                                 |
         *               add message           .            `---`                             |
         *                    |                .                |                             |
         *                    v                .             hasMessage? -----------.         |
         *                  awake              .                |                   |         |
         *                    |                .                v                   v         |
         *                    v                .              wait -1        hasMessage=false |
         *           lock pollOnce mutex       .                |                   |         |
         *                    |                .            .---+                   |         |
         *                    v                .            v                       |         |
         *            hasMessage=true          .        Lock header <---- return <--+         |
         *                    |                .            |                                 |
         *                    v                .            v                                 |
         *               notify_all            .           ....                               |
         *                    |                .            |                                 |
         *                    v                .            v                                 |
         *           unlock pollOnce mutex     .         Unlock +-----------------------------+
         */

        PollOnce(wait_mills);

        if (is_quited_) {
            break;
        }

        {
            // 保证头结点的线程安全
            SmartRecursiveLock lock(header_mutex_);

            Message *msg = header_;
            if (msg != nullptr) {
                auto now = ThreadUtil::TimeMills();
                if (now < msg->when_) {
                    // 等待
                    wait_mills = msg->when_ - now;
                } else {
                    // 无需等待，直接返回头结点
                    header_ = msg->next_;
                    msg->next_ = nullptr;
                    return msg;
                }
            } else {
                wait_mills = -1;
            }
        }

        if (is_quited_) {
            break;
        }
    }

    return nullptr;
}


void MessageQueue::PollOnce(long long wait_mills) {
    if (wait_mills == 0) {
        return;
    }

    {
        std::unique_lock<std::mutex> lock(poll_once_mutex_);

        if (wait_mills < 0) {
            if (has_message_) {
                has_message_ = false;
                return;
            }
            condition_.wait(lock);
//            logger.i(TAG, "pollOnce 结束无限等待");
            return;
        }

        condition_.wait_for(lock, std::chrono::milliseconds(wait_mills));
//        logger.i(TAG, "pollOnce 结束等待");
    }
}

void MessageQueue::Quit(bool is_safely) {
    if (is_quited_) {
        return;
    }

    {
        std::unique_lock<std::mutex> lock(quit_mutex_);
        is_quited_ = true;
    }

    if (is_safely) {
        RemoveAllMessagesSafely();
    } else {
        RemoveAllMessagesNotSafely();
    }

    // 如果线程处于 wait 状态则唤醒线程（否则线程不退出）
    Awake();
}

void MessageQueue::RemoveAllMessagesSafely() {

    // 保证头结点的线程安全
    SmartRecursiveLock lock(header_mutex_);

    Message *p = header_;
    if (p == nullptr) {
        return;
    }

    Timestamp now = ThreadUtil::TimeMills();

    // 如果头结点的执行时间都要比当前时刻晚，可以全部移除
    if (p->when_ > now) {
        RemoveAllMessagesNotSafely();
        return;
    }

    // 找第一个执行时间晚于当前时刻的消息
    Message *first_need_remove_msg;
    while (true) {
        first_need_remove_msg = p->next_;
        if (first_need_remove_msg == nullptr) {
            // 没找到
            return;
        }
        if (first_need_remove_msg->when_ > now) {
            break;
        }
        p = first_need_remove_msg;
    }
    p->next_ = nullptr;

    // 回收后面的所有消息
    do {
//        logger.i("MessageQueue", "回收消息[1]");
        p = first_need_remove_msg;
        first_need_remove_msg = p->next_;
        p->Recycle();
    } while (first_need_remove_msg != nullptr);
}

void MessageQueue::RemoveAllMessagesNotSafely() {
    // 保证头结点的线程安全
    SmartRecursiveLock header_safe(header_mutex_);

    Message *p = header_;
    while (p != nullptr) {
//        logger.i(TAG, "回收消息[2]");
        auto n = p->next_;
        p->Recycle();
        p = n;
    }
    header_ = nullptr;
}

void MessageQueue::SetName(const std::string &name) {
    this->name_ = name;
}