////
//// Created by boss on 2023/11/27.
////

#include <thread>

#include <gtest/gtest.h>

#include "thread/thread_log.h"
#include "thread/thread_util.h"
#include "timer/timer.h"

#define TAG "TimerTest"

TEST(TimerTest, StartDelay) {
    bool called = false;
    const unsigned long long ms1 = ThreadUtil::TimeMills();
    unsigned long long ms2;
    Timer timer([&]() {
        called = true;
        ms2 = ThreadUtil::TimeMills();
    });
    timer.StartDelay(100);
    sleep(1);
    EXPECT_TRUE(called);
    EXPECT_NEAR(ms1 + 100, ms2, 100);
}

TEST(TimerTest, StartAt) {
    bool called = false;
    const unsigned long long ms1 = ThreadUtil::TimeMills();
    unsigned long long ms2;
    Timer timer([&]() {
        called = true;
        ms2 = ThreadUtil::TimeMills();
    });
    timer.StartAt(ms1 + 100);
    sleep(1);
    EXPECT_TRUE(called);
    EXPECT_NEAR(ms1 + 100, ms2, 100);
}

TEST(TimerTest, Cancel) {
    bool called = false;
    const unsigned long long ms1 = ThreadUtil::TimeMills();
    unsigned long long ms2;
    Timer timer([&]() {
        called = true;
        ms2 = ThreadUtil::TimeMills();
    });
    timer.StartAt(ms1 + 100);
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    timer.Cancel();
    sleep(1);
    EXPECT_FALSE(called);
}
