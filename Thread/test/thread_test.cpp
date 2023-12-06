////
//// Created by boss on 2023/11/27.
////

#include <future>

#include <gtest/gtest.h>

#include "thread/handler_thread.h"
#include "thread/leak_companion.h"
#include "thread/loop_thread.h"
#include "thread/smart_thread.h"
#include "thread/smart_loop_thread.h"
#include "thread/thread.h"
#include "thread/thread_local.h"
#include "thread/thread_log.h"
#include "thread/thread_pool.h"

#define TAG "ThreadTest"

TEST(ThreadTest, LeakCompanion) {
    ASSERT_EQ(LeakCompanion::GetCurrentCount(), 0);
    {
        LeakCompanion comp1("comp1");
        ASSERT_EQ(LeakCompanion::GetCurrentCount(), 1);
    }
    ASSERT_EQ(LeakCompanion::GetCurrentCount(), 0);
    {
        LeakCompanion comp1("comp1");
        ASSERT_EQ(LeakCompanion::GetCurrentCount(), 1);
        LeakCompanion::AssertCount(2);
        LeakCompanion comp2("comp2");
        ASSERT_EQ(LeakCompanion::GetCurrentCount(), 2);
    }
    ASSERT_EQ(LeakCompanion::GetCurrentCount(), 0);
}

TEST(ThreadTest, ThreadLocal) {
    {
        std::vector<std::future<int>> futures;
        for (int i = 0; i < 5; ++i) {
            auto runnable = [i]() {
                ThreadLocal<int> local1;
                int num = i;
                local1.Set(&num);
                EXPECT_EQ(*local1.Get(), num);
                return *local1.Get();
            };
            futures.emplace_back(std::move(std::async(runnable)));
        }

        for (int i = 0; i < futures.size(); ++i) {
            EXPECT_EQ(futures.at(i).get(), i);
        }
    }

    {
        class A {
        public:
            explicit A(int num) : num_(num) {}

        public:
            int num_;
        };

        std::vector<std::future<A *>> futures;
        for (int i = 0; i < 5; ++i) {
            auto runnable = [i]() {
                ThreadLocal<A> local2;
                auto obj = new A(i * 10);
                local2.Set(obj);
                EXPECT_EQ(local2.Get()->num_, i * 10);
                return local2.Get();
            };
            futures.emplace_back(std::move(std::async(runnable)));
        }

        for (int i = 0; i < futures.size(); ++i) {
            auto obj = futures.at(i).get();
            EXPECT_EQ(obj->num_, i * 10);
            delete obj;
        }
    }
}

TEST(ThreadTest, Thread) {
    auto t1 = Thread::CurrentThread();
    EXPECT_TRUE(t1 == nullptr);

    Thread t2([]() {
        for (int i = 0; i < 5; ++i) {
            logger.i(TAG, "线程2休眠中...");
            Thread::Sleep(1 * 100);
        }

        auto t3 = Thread::CurrentThread();
        EXPECT_TRUE(t3 != nullptr);
        EXPECT_STREQ(t3->GetName().c_str(), "T2");
        if (t3) {
            logger.i(TAG, t3->GetName());
            printf("[2] 当前线程: %p\n", t3);
        } else {
            logger.i(TAG, "thread == nullptr");
        }

    });
    t2.SetName("T2");
    t2.SetOnFinishCallback([]() {
        logger.i(TAG, "Thread end.");
    });
    t2.Start();
    Thread::Sleep(1000);

    auto t3 = Thread::CurrentThread();
    EXPECT_TRUE(t3 == nullptr);
    if (t3) {
        logger.i(TAG, t3->GetName());
    } else {
        logger.i(TAG, "thread == nullptr");
    }
}

TEST(SmartThreadTest, Test) {
    bool called = false;
    auto t1 = SmartThread::NewInstance([&]() {
        called = true;
        logger.i(TAG, "SmartThread 1 正在运行");
        for (int i = 0; i < 3; ++i) {
            Thread::Sleep(100);
        }
    });
    t1->SetName("ST1");
    t1->SetOnFinishCallback([]() {
        logger.i(TAG, "SmartThread 1 准备结束了");
    });
    t1->Start();

    Thread::Sleep(1000);
    EXPECT_TRUE(called);
}

TEST(LoopThreadTest, Test) {
    LoopThread t1;
    t1.SetName("LP1");

    t1.Post([]() {
        logger.i(TAG, "正在执行 1");
        Thread::Sleep(100);
        logger.i(TAG, "1 执行结束");
    });

    t1.Post([]() {
        logger.i(TAG, "正在执行 2");
        Thread::Sleep( 100);
        logger.i(TAG, "2 执行结束");
    });

    t1.SetOnFinishCallback([]() {
        logger.i(TAG, "结束啦");
    });

    t1.Start();
    Thread::Sleep(1 * 1000);
    t1.Post([]() {
        logger.i(TAG, "正在执行 3");
        Thread::Sleep(100);
        logger.i(TAG, "3 执行结束");
    });
    Thread::Sleep(1000);
    logger.i(TAG, "准备停止" + t1.GetName());
    t1.TryStop();
}

TEST(SmartLoopThreadTest, Test) {
    auto t1 = SmartLoopThread::NewInstance();
    t1->SetName("T1");
    t1->Post([]() {
        logger.i(TAG, "正在执行 1");
        Thread::Sleep(100);
        logger.i(TAG, "1 执行结束");
    });
    t1->Post([]() {
        logger.i(TAG, "正在执行 2");
        Thread::Sleep(100);
        logger.i(TAG, "2 执行结束");
    });
    t1->SetOnFinishCallback([]() {
        logger.i(TAG, "要结束啦");
        Thread::Sleep(100);
    });

    t1->Start();
    logger.i(TAG, t1->GetName() + " 线程已经启动！");
    Thread::Sleep(1000);

    t1->Post([]() {
        logger.i(TAG, "正在执行 3");
        Thread::Sleep(100);
        logger.i(TAG, "3 执行结束");
    });
    Thread::Sleep(1000);

    t1->TryStop();
    logger.i(TAG, t1->GetName() + " 线程将要停止");
}

TEST(ThreadPoolTest, Test) {
    auto threadPool = ThreadPool::NewInstance("TP1", 3);

    threadPool->Execute("Task1", []() {
        logger.i(TAG, "任务1正在被执行...");
        Thread::Sleep(100);
        logger.i(TAG, "任务1执行结束...");
    });

    threadPool->Execute("Task2", []() {
        for (int i = 0; i < 2; ++i) {
            logger.i(TAG, "任务2正在被执行 | %d", i + 1);
            Thread::Sleep(100);
        }
    });

    threadPool->Execute("Task3", []() {
        for (int i = 0; i < 3; ++i) {
            logger.i(TAG, "任务3正在被执行 | %d", i + 1);
            Thread::Sleep(100);
        }
    });

    threadPool->Execute("Task4", []() {
        for (int i = 0; i < 3; ++i) {
            logger.i(TAG, "任务4正在被执行 | %d", i + 1);
            Thread::Sleep(100);
        }
    });

    logger.i(TAG, "线程池大小: %d", threadPool->GetThreadCount());

    Thread::Sleep(2000);
    threadPool->ShutDownAndDeleteSelf();
    threadPool = nullptr;

    Thread::Sleep(1000);

    logger.i(TAG, "主线程休眠中...");
    Thread::Sleep(1000);
}


TEST(HandlerThreadTest, Test) {
    auto thread = new HandlerThread("HandlerThread1");
    thread->Start();

    thread->GetHandler()->Post([]() {
        logger.i(TAG, "【1】HandlerThread正在执行...");
    });

    Thread::Sleep(1000);
    thread->QuitThenDeleteSelf(true);

    logger.i(TAG, "主线程休眠中...");
    Thread::Sleep(1000);
}