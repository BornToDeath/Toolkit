//
// Created by boss on 2023/11/27.
//

#include <mutex>
#include <thread>

#include <gtest/gtest.h>

#include "lock/smart_lock.h"
#include "lock/smart_recursive_lock.h"
#include "lock/wait_helper.h"
#include "thread/thread_util.h"

TEST(LockTest, SmartLock) {
    int num = 0;
    std::mutex mutex;
    auto runnable = [&mutex, &num](const std::string &name) {
        for (int i = 0; i < 10; ++i) {
            SmartLock lock(mutex, name);
            ++num;
            printf("%s|num=%d\n", name.c_str(), num);
        }
    };
    std::thread t1(runnable, "lock1");
    std::thread t2(runnable, "lock1");
    t1.join();
    t2.join();
    EXPECT_EQ(num, 20);
    printf("------ End ------\n");
}

void loop(int &num, std::recursive_mutex &rmutex, const std::string &name) {
    if (num == 10) {
        return;
    }
    SmartRecursiveLock lock(rmutex, name);
    ++num;
    printf("%s|num=%d\n", name.c_str(), num);
    loop(num, rmutex, name);
}

TEST(LockTest, SmartRecursiveLock) {
    int num = 0;
    std::recursive_mutex rmutex;
    loop(num, rmutex, "smart_recursive_lock");
    EXPECT_EQ(num, 10);
    printf("------ End ------\n");
}

TEST(LockTest, WaitHelper) {
    int num = 0;
    WaitHelper wait_helper;
    std::thread([&num, &wait_helper] {
        for (; num < 10; ++num) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            wait_helper.Notify();
        }
    }).detach();
    wait_helper.WaitUntil([&num]() { return num == 10; });
    EXPECT_EQ(num, 10);

    const auto ms1 = ThreadUtil::TimeMills();
    wait_helper.WaitFor(1000, [&num]() { return num == 20; });
    const auto ms2 = ThreadUtil::TimeMills();
    EXPECT_TRUE(ms2 - ms1 >= 1000);
    EXPECT_NE(num, 20);

    EXPECT_TRUE(wait_helper.TryLock());
    std::thread([&wait_helper]() {
        wait_helper.Lock();
        std::this_thread::sleep_for(std::chrono::seconds(3));
        wait_helper.Unlock();
    }).detach();
    EXPECT_FALSE(wait_helper.TryLock());

    printf("------ End ------\n");
}
