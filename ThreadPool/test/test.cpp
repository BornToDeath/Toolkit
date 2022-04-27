//
// Created by lixiaoqing on 2022/4/27.
//

#include "ThreadPool.h"
#include <iostream>

namespace Test {
    void testThreadPool() {
        auto runnable = [](int num) {
            printf("TID=%llu | num=%d\n", std::this_thread::get_id(), num);
            std::this_thread::sleep_for(std::chrono::seconds(1));
            return num;
        };

        std::vector<std::future<int>> results;
        {
            ThreadPool pool(4);
            for (int i = 1; i <= 10; ++i) {
                std::future<int> f = pool.enqueue(runnable, i);
                results.emplace_back(std::move(f));
            }
        }
        printf("----------------------------\n");
        for (auto &&result: results)
            printf("%d, ", result.get());
        std::cout << std::endl;
    }
}

int main() {
    Test::testThreadPool();
    while (true) {
        printf("主线程休眠中...\n");
        std::this_thread::sleep_for(std::chrono::seconds(10));
    }
    return 0;
}