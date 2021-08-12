//
// Created by lixiaoqing on 2021/8/12.
//

#include <sys/prctl.h>
#include "Thread/Thread.h"
#include "Thread/ThreadLog.h"
#include "Thread/SmartThread.h"
#include "Thread/LoopThread.h"

#define TAG "Test"

void threadTest();

void smartThreadTest();

void loopThreadTest();


int main() {
    prctl(PR_SET_NAME, "MainThread");

    // 测试
//    threadTest();
//    smartThreadTest();
    loopThreadTest();

    logger.i(TAG, "主线程休眠中...");
    pthread_exit(nullptr);
    return 0;
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
    Thread::sleep(1*1000);
    t1.post([]() {
        logger.i(TAG, "正在执行 3");
        Thread::sleep(1 * 1000);
        logger.i(TAG, "3 执行结束");
    });
    Thread::sleep(5*1000);
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
