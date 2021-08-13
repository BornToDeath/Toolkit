//
// Created by lixiaoqing on 2021/8/13.
//

#include "Message/MessageQueue.h"
#include "Thread/ThreadLog.h"
#include "Thread/ThreadUtil.h"
#include "Lock/SmartRecursiveLock.h"

#define TAG "MessageQueue"


MessageQueue::MessageQueue() : isQuited(false), hasMessage(false), header(nullptr) {

}

MessageQueue::~MessageQueue() {
    logger.i(TAG, "~MessageQueue() called.");
}

void MessageQueue::enqueueMessage(Message *message, Timestamp when) {
    if (isQuited || message == nullptr) {
        return;
    }

    {
        logger.i(TAG, "enqueue a message.");

        // 保证头结点的线程安全
        SmartRecursiveLock lock(headerMutex);

        auto p = header;
        if (p == nullptr || when == 0 || when < p->when) {
            message->next = p;
            header = message;
        } else {
            // 找到要插入的位置
            Message *prev;
            while (true) {
                prev = p;
                p = p->next;
                if (p == nullptr || when < p->when) {
                    break;
                }
            }
            // 插入到 prev 和 p 之间
            message->next = p;
            prev->next = message;
        }
    }

    // 尝试通知一下
    awake();
}

void MessageQueue::awake() {
    logger.i(TAG, "awake");
    std::unique_lock<std::mutex> lock(pollOnceMutex);
    hasMessage = true;
    condition.notify_all();
}

Message *MessageQueue::next() {
    Timestamp waitMills = 0;

    while (true) {

        /*
         * 如果在 next 的 unlock 之后，wait 之前，另一个线程调用了 enqueueMessage，
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
         *            hasMessage=true          .        lock header <---- return <--+         |
         *                    |                .            |                                 |
         *                    v                .            v                                 |
         *               notify_all            .           ....                               |
         *                    |                .            |                                 |
         *                    v                .            v                                 |
         *           unlock pollOnce mutex     .         unlock +-----------------------------+
         */

        pollOnce(waitMills);

        if (isQuited) {
            break;
        }

        {
            // 保证头结点的线程安全
            SmartRecursiveLock lock(headerMutex);

            Message *msg = header;
            if (msg != nullptr) {
                auto now = ThreadUtil::timeMills();
                if(now < msg->when) {
                    // 等待
                    waitMills = msg->when - now;
                } else {
                    // 无需等待，直接返回头结点
                    header = msg->next;
                    msg->next = nullptr;
                    return msg;
                }
            } else {
                waitMills = -1;
            }
        }

        if (isQuited) {
            break;
        }
    }

    return nullptr;
}


void MessageQueue::pollOnce(long long waitMills) {
    if (waitMills == 0) {
        return;
    }

    {
        std::unique_lock<std::mutex> lock(pollOnceMutex);

        if (waitMills < 0) {
            if (hasMessage) {
                hasMessage = false;
                return;
            }
            condition.wait(lock);
            logger.i(TAG, "pollOnce 结束无限等待");
            return;
        }

        condition.wait_for(lock, std::chrono::milliseconds(waitMills));
        logger.i(TAG, "pollOnce 结束等待");
    }
}

void MessageQueue::quit(bool isSafely) {
    if (isQuited) {
        return;
    }

    {
        std::unique_lock<std::mutex> lock(quitMutex);
        isQuited = true;
    }

    if (isSafely) {
        removeAllMessagesSafely();
    } else {
        removeAllMessagesNotSafely();
    }

    // 如果线程处于 wait 状态则唤醒线程（否则线程不退出）
    awake();
}

void MessageQueue::removeAllMessagesSafely() {

    // 保证头结点的线程安全
    SmartRecursiveLock lock(headerMutex);

    Message* p = header;
    if (p == nullptr) {
        return;
    }

    Timestamp now = ThreadUtil::timeMills();

    // 如果头结点的执行时间都要比当前时刻晚，可以全部移除
    if (p->when > now) {
        removeAllMessagesNotSafely();
        return;
    }

    // 找第一个执行时间晚于当前时刻的消息
    Message* firstNeedRemoveMsg;
    while (true) {
        firstNeedRemoveMsg = p->next;
        if (firstNeedRemoveMsg == nullptr) {
            // 没找到
            return;
        }
        if (firstNeedRemoveMsg->when > now) {
            break;
        }
        p = firstNeedRemoveMsg;
    }
    p->next = nullptr;

    // 回收后面的所有消息
    do {
        logger.i("MessageQueue", "回收消息[1]");
        p = firstNeedRemoveMsg;
        firstNeedRemoveMsg = p->next;
        p->recycle();
    } while (firstNeedRemoveMsg != nullptr);
}

void MessageQueue::removeAllMessagesNotSafely() {
    // 保证头结点的线程安全
    SmartRecursiveLock headerSafe(headerMutex);

    Message* p = header;
    while (p != nullptr) {
        logger.i(TAG, "回收消息[2]");
        auto n = p->next;
        p->recycle();
        p = n;
    }
    header = nullptr;
}