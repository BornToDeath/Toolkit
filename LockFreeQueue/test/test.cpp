//
// Created by lixiaoqing on 2022/10/25.
//

#include <iostream>
#include <memory>
#include <queue>
#include <thread>

#include "ring_buffer.h"
#include "lock_free_queue.h"

namespace Test {
    void ringBufferTest() {
        auto ringBuffer = std::make_shared<RingBuffer<int>>(10);
        std::cout << ringBuffer->IsEmpty() << ", " << ringBuffer->IsFull() << std::endl;

//        for (int i = 1; i <= 10; ++i) {
//            ringBuffer->Push(i);
//            std::cout << ringBuffer->IsEmpty() << ", " << ringBuffer->IsFull() << std::endl;
//            std::cout << "Tail=" << *ringBuffer->Tail() << std::endl;
//        }

        auto produce = [ringBuffer]() {
            for (int i = 1; i <= 1000; ++i) {
                auto res = ringBuffer->Push(i);
                printf("[producer] %d, res=%d\n", i, res);
            }
            printf("[producer] over\n");
        };

        auto consume = [ringBuffer]() {
            while (true) {
                if (!ringBuffer->IsEmpty()) {
                    printf("[consumer] %d\n", ringBuffer->Pop());
                }
            }
            printf("[consumer] over\n");
        };

        std::thread t1(produce);
        std::thread t2(consume);
        t1.join();
        t2.join();
    }

    void lockfreeQueueTest() {
        const int SIZE = 100;
        auto q = std::make_shared<LockFreeQueue<int>>(10, "/lxq8");

        auto produce = [q]() {
            for (int i = 1; i <= SIZE; ++i) {
                auto res = q->Push(i);
                printf("[PRODUCER] res=%d, i=%d, Head=%d, Tail=%d\n", res, i, q->Head(), q->Tail());
            }
            printf("[PRODUCER] over\n");
        };

        auto consume = [q](int name) {
            while (true) {
                if (!q->IsEmpty()) {
                    int e=-1;
                    auto res = q->Pop(e);
                    printf("[CONSUMER] res=%d, name=%d, e=%d\n", res, name, e);
                }
            }
        };

        std::thread t1(produce);
        std::thread t2(consume, 1);
        std::thread t3(consume, 2);
        t1.join();
        t2.join();
        t3.join();
    }
}

int main() {
//    Test::ringBufferTest();
    Test::lockfreeQueueTest();
    return 0;
}