//
// Created by lixiaoqing on 2022/4/27.
//

#include <iostream>

#include <gtest/gtest.h>

#include "thread_pool.h"

class ThreadPoolTest : public testing::Test {
public:
    void SetUp() override {
        pool_ = std::make_unique<ThreadPool>(4);
    }

    void TearDown() override {
        pool_.reset();
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
        for (int i = 0; i < 10; ++i) {
            auto &&f = pool_->enqueue(runnable, i);
            EXPECT_TRUE(f.valid());
            results.emplace_back(std::move(f));
        }
    }

    for (int i = 0; i < results.size(); ++i) {
        EXPECT_EQ(results.at(i).get(), i);
    }
}

TEST_F(ThreadPoolTest, ThreadPool2) {
    pool_ = std::make_unique<ThreadPool>(4);
    EXPECT_FALSE(pool_->is_stop_);
    EXPECT_EQ(pool_->workers_.size(), 4);
    auto &&f = pool_->enqueue([]() { return 10; });
    EXPECT_TRUE(f.valid());
    printf("future: %d\n", f.get());
}

TEST(ThreadPoolTest2, ThreadPool) {
    ThreadPool pool(-1);
    auto &&future = pool.enqueue([]() { return 10; });
    EXPECT_FALSE(future.valid());
}

//TEST(ThreadPoolTask, ThreadPool) {

//}