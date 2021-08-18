//
// Created by lixiaoqing on 2021/8/12.
//

#include <sys/prctl.h>
#include "Thread/Thread.h"
#include "Thread/ThreadLog.h"
#include "Thread/SmartThread.h"
#include "Thread/LoopThread.h"
#include "Thread/SmartLoopThread.h"
#include "Message/Message.h"
#include "Message/MessageQueue.h"
#include "Looper/Looper.h"
#include "Thread/HandlerThread.h"
#include "ThreadPool/ThreadPool.h"

#define TAG "Test"

void threadTest();

void smartThreadTest();

void loopThreadTest();

void smartLoopThreadTest();

void messageTest();

void messageQueueTest();

void looperTest();

void handlerTest();

void handlerThreadTest();

void threadPoolTest();


int main() {
    prctl(PR_SET_NAME, "MainThread");

    // 测试
//    threadTest();
//    smartThreadTest();
//    loopThreadTest();
//    smartLoopThreadTest();
//    messageTest();
//    messageQueueTest();
//    looperTest();
//    handlerTest();
//    handlerThreadTest();
    threadPoolTest();

    logger.i(TAG, "主线程休眠中...");
//    pthread_exit(nullptr);
    return 0;
}

void threadPoolTest() {
    auto threadPool = ThreadPool::newThreadPool("TP1", 3);

    threadPool->execute("Task1", []() {
        logger.i(TAG, "任务1正在被执行...");
        Thread::sleep(1 * 1000);
        logger.i(TAG, "任务1执行结束...");
    });

    threadPool->execute("Task2", []() {
        for (int i = 0; i < 2; ++i) {
            logger.i(TAG, "任务2正在被执行 | %d", i + 1);
            Thread::sleep(1 * 1000);
        }
    });

    threadPool->execute("Task3", []() {
        for (int i = 0; i < 3; ++i) {
            logger.i(TAG, "任务3正在被执行 | %d", i + 1);
            Thread::sleep(1 * 1000);
        }
    });

    threadPool->execute("Task4", []() {
        for (int i = 0; i < 3; ++i) {
            logger.i(TAG, "任务4正在被执行 | %d", i + 1);
            Thread::sleep(1 * 1000);
        }
    });

    logger.i(TAG, "线程池大小: %d", threadPool->getThreadCount());

    Thread::sleep(5 * 1000);
    threadPool->shutDownAndDeleteSelf();
    threadPool = nullptr;

    Thread::sleep(1*1000);

    while (true) {
        logger.i(TAG, "主线程休眠中...");
        Thread::sleep(3 * 1000);
    }
}

void handlerThreadTest() {
    auto thread = new HandlerThread("HandlerThread1");
    thread->start();

    thread->getHandler()->post([]() {
        logger.i(TAG, "【1】HandlerThread正在执行...");
    });

    Thread::sleep(1 * 1000);
    thread->quitThenDeleteSelf(true);

    while (true) {
        logger.i(TAG, "主线程休眠中...");
        Thread::sleep(3 * 1000);
    }
}

void handlerTest() {
    Looper *looper = nullptr;
    Thread t1([&looper]() {
        auto flag = Looper::prepare();
        logger.i(TAG, "flag: %d", flag);
        looper = Looper::myLooper();
        Looper::loop();
    });
    t1.setName("LooperThread");
    t1.start();
    Thread::sleep(1 * 1000);

    auto handler = new Handler(looper);
    handler->post([]() {
        logger.i(TAG, "【1】我是一条消息，我被执行了");
    });
    handler->postDelay([]() {
        logger.i(TAG, "【2】我是一条消息，我被延迟执行了");
    }, 2000);

    Thread::sleep(3 * 1000);
    looper->quitSafely();
    logger.i(TAG, "退出 looper");

    while (true) {
        logger.i(TAG, "主线程休眠中...");
        Thread::sleep(3 * 1000);
    }
}

void looperTest() {
    Looper *looper = nullptr;
    Thread t1([&looper]() {
        auto flag = Looper::prepare();
        logger.i(TAG, "flag: %d", flag);
        looper = Looper::myLooper();
        Looper::loop();
    });
    t1.setName("LooperThread");
    t1.start();
    Thread::sleep(1 * 1000);

//    auto looper = Looper::myLooper();
    if (looper == nullptr) {
        logger.i(TAG, "Looper is null.");
    }

    auto handler = new Handler(looper);

    auto msg1 = Message::obtain();
    msg1->what = 10;
    msg1->target = handler;
    looper->queue->enqueueMessage(msg1, 0);

    auto msg2 = Message::obtain();
    msg2->what = 20;
    msg2->target = handler;
    looper->queue->enqueueMessage(msg2, 0);

    Thread::sleep(3 * 1000);
    looper->quitSafely();
    delete handler;

    while (true) {
        logger.i(TAG, "主线程休眠中...");
        Thread::sleep(3 * 1000);
    }
}

void messageQueueTest() {

    static bool isQuit = false;
    MessageQueue queue;

    auto msg1 = Message::obtain();
    msg1->what = 10;
    queue.enqueueMessage(msg1, 0);

    auto msg2 = Message::obtain();
    msg2->what = 20;
    queue.enqueueMessage(msg2, 0);

    auto runnable = [&queue]() {
        while (!isQuit) {
            // 取消息
            logger.i(TAG, "准备取消息...");
            auto newMsg = queue.next();
            if (newMsg) {
                logger.i(TAG, "what: " + std::to_string(newMsg->what));
            } else {
                logger.i(TAG, "没有获取到消息！");
            }
        }
    };

    Thread t1(runnable);
    t1.setName("T1");
    t1.start();

    for (int i = 1; i <= 1; ++i) {
        // 发消息
        auto msgg = Message::obtain();
        msgg->what = i + 1;
        queue.enqueueMessage(msgg, 0);
        Thread::sleep(1 * 1000);
    }

    // 退出消息队列
    logger.i(TAG, "MessageQueue 准备退出");
    isQuit = true;
    queue.quit(true);
}

void messageTest() {
    auto message = Message::obtain();
    message->recycle();
}

void smartLoopThreadTest() {
    auto t1 = SmartLoopThread::newThread();
    t1->setName("T1");
    t1->post([]() {
        logger.i(TAG, "正在执行 1");
        Thread::sleep(1 * 1000);
        logger.i(TAG, "1 执行结束");
    });
    t1->post([]() {
        logger.i(TAG, "正在执行 2");
        Thread::sleep(1 * 1000);
        logger.i(TAG, "2 执行结束");
    });
    t1->setOnFinishCallback([]() {
        logger.i(TAG, "要结束啦");
        Thread::sleep(1 * 1000);
    });

    t1->start();
    logger.i(TAG, t1->getName() + " 线程已经启动！");
    Thread::sleep(3 * 1000);

    t1->post([]() {
        logger.i(TAG, "正在执行 3");
        Thread::sleep(1 * 1000);
        logger.i(TAG, "3 执行结束");
    });
    Thread::sleep(3 * 1000);

    t1->tryStop();
    logger.i(TAG, t1->getName() + " 线程将要停止");
}

void loopThreadTest() {
    LoopThread t1;
    t1.setName("LP1");

    t1.post([]() {
        logger.i(TAG, "正在执行 1");
        Thread::sleep(1 * 1000);
        logger.i(TAG, "1 执行结束");
    });

    t1.post([]() {
        logger.i(TAG, "正在执行 2");
        Thread::sleep(1 * 1000);
        logger.i(TAG, "2 执行结束");
    });

    t1.setOnFinishCallback([]() {
        logger.i(TAG, "结束啦");
    });

    t1.start();
    Thread::sleep(1 * 1000);
    t1.post([]() {
        logger.i(TAG, "正在执行 3");
        Thread::sleep(1 * 1000);
        logger.i(TAG, "3 执行结束");
    });
    Thread::sleep(5 * 1000);
    logger.i(TAG, "准备停止" + t1.getName());
    t1.tryStop();
}

void smartThreadTest() {
    auto t1 = SmartThread::newThread([]() {
        logger.i(TAG, "SmartThread 1 正在运行");
        for (int i = 0; i < 5; ++i) {
            Thread::sleep(1 * 1000);
        }
    });
    t1->setName("ST1");
    t1->setOnFinishCallback([]() {
        logger.i(TAG, "SmartThread 1 准备结束了");
    });
    t1->start();
}

void threadTest() {
    logger.i(TAG, "Thread Test.");
    auto t1 = Thread::currentThread();
    if (t1) {
        logger.i(TAG, t1->getName());
    } else {
        logger.i(TAG, "Thread is null");
    }

    Thread t2([]() {
        for (int i = 0; i < 5; ++i) {
            logger.i(TAG, "线程2休眠中...");
            Thread::sleep(1 * 1000);
        }

        auto t3 = Thread::currentThread();
        if (t3) {
            logger.i(TAG, t3->getName());
            printf("[2] 当前线程: %p\n", t3);
        } else {
            logger.i(TAG, "thread == nullptr");
        }

    });
    t2.setName("T2");
    t2.setOnFinishCallback([]() {
        logger.i(TAG, "Thread end.");
    });
    t2.start();

    auto t3 = Thread::currentThread();
    if (t3) {
        logger.i(TAG, t3->getName());
    } else {
        logger.i(TAG, "thread == nullptr");
    }
}
