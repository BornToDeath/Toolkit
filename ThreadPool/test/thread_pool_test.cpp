//
// Created by lixiaoqing on 2022/4/27.
//

#include <iostream>

#include <gtest/gtest.h>

#include "thread_pool.h"

class ThreadPoolTest : public testing::Test {
public:
    void SetUp() override {

    }

    void TearDown() override {

    }

public:
    std::unique_ptr<ThreadPool> pool_;
};

TEST_F(ThreadPoolTest, ThreadPool) {
    auto runnable = [](int num) {
        printf("TID=%llu | num=%d\n", std::this_thread::get_id(), num);
        std::this_thread::sleep_for(std::chrono::seconds(1));
        return num;
    };

    std::vector<std::future<int>> results;
    {
        ThreadPool pool(4);
        for (int i = 0; i < 10; ++i) {
            std::future<int> f = pool.enqueue(runnable, i);
            results.emplace_back(std::move(f));
        }
    }

    for (int i = 0; i < results.size(); ++i) {
        EXPECT_EQ(results.at(i).get(), i);
    }
}

TEST_F(ThreadPoolTest, ThreadPool2) {
    pool_ = std::make_unique<ThreadPool>(4);
    EXPECT_FALSE(pool_->stop_);
    EXPECT_EQ(pool_->workers_.size(), 4);
    auto future1 = pool_->enqueue([]() { return 10; });
    printf("future1: %d", future1.get());
}

//TEST(ThreadPoolTask, ThreadPool) {

//}