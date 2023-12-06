////
//// Created by boss on 2023/11/27.
////

#include <gtest/gtest.h>

#include "message/handler.h"
#include "message/looper.h"
#include "message/message.h"
#include "message/message_queue.h"
#include "thread/thread.h"
#include "thread/thread_log.h"

#define TAG "MessageTest"

TEST(MessageTest, Test) {
    auto message = Message::Obtain();
    EXPECT_TRUE(message != nullptr);
    message->Recycle();
}

TEST(MessageQueueTest, Test) {
    static bool isQuit = false;
    MessageQueue queue;

    auto msg1 = Message::Obtain();
    msg1->what_ = 10;
    queue.EnqueueMessage(msg1, 0);

    auto msg2 = Message::Obtain();
    msg2->what_ = 20;
    queue.EnqueueMessage(msg2, 0);

    auto runnable = [&queue]() {
        while (!isQuit) {
            // 取消息
            logger.i(TAG, "准备取消息...");
            auto newMsg = queue.Next();
            if (newMsg) {
                logger.i(TAG, "what: " + std::to_string(newMsg->what_));
            } else {
                logger.i(TAG, "没有获取到消息！");
            }
        }
    };

    Thread t1(runnable);
    t1.SetName("T1");
    t1.Start();

    for (int i = 1; i <= 1; ++i) {
        // 发消息
        auto msgg = Message::Obtain();
        msgg->what_ = i + 1;
        queue.EnqueueMessage(msgg, 0);
        Thread::Sleep(100);
    }

    // 退出消息队列
    logger.i(TAG, "MessageQueue 准备退出");
    isQuit = true;
    queue.Quit(true);
}

TEST(LooperTest, Test) {
    Looper *looper = nullptr;
    Thread t1([&looper]() {
        auto flag = Looper::Prepare();
        logger.i(TAG, "flag: %d", flag);
        looper = Looper::MyLooper();
        Looper::Loop();
    });
    t1.SetName("LooperThread");
    t1.Start();
    Thread::Sleep(100);

//    auto looper = Looper::MyLooper();
    if (looper == nullptr) {
        logger.i(TAG, "Looper is null.");
    }

    auto handler = new Handler(looper);

    auto msg1 = Message::Obtain();
    msg1->what_ = 10;
    msg1->target_ = handler;
    looper->queue_->EnqueueMessage(msg1, 0);

    auto msg2 = Message::Obtain();
    msg2->what_ = 20;
    msg2->target_ = handler;
    looper->queue_->EnqueueMessage(msg2, 0);

    Thread::Sleep(1000);
    looper->QuitSafely();
    delete handler;

    logger.i(TAG, "主线程休眠中...");
    Thread::Sleep(1000);
}

TEST(HandlerTest, Test) {
    Looper *looper = nullptr;
    Thread t1([&looper]() {
        auto flag = Looper::Prepare();
        logger.i(TAG, "flag: %d", flag);
        looper = Looper::MyLooper();
        Looper::Loop();
    });
    t1.SetName("LooperThread");
    t1.Start();
    Thread::Sleep(1 * 1000);

    auto handler = new Handler(looper);
    handler->Post([]() {
        logger.i(TAG, "【1】我是一条消息，我被执行了");
    });
    handler->PostDelay([]() {
        logger.i(TAG, "【2】我是一条消息，我被延迟执行了");
    }, 100);

    Thread::Sleep(1000);
    looper->QuitSafely();
    logger.i(TAG, "退出 looper");

    logger.i(TAG, "主线程休眠中...");
    Thread::Sleep(1000);
}
