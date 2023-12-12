//
// Created by lixiaoqing on 2021/7/15.
//


#include <iostream>

#include "threads.h"
#include "thread_dispatcher.h"


void sleep(int secs) {
    std::this_thread::sleep_for(std::chrono::seconds(secs));
}

void testThreadDispatcher();

void testThreads();


int main() {
    testThreadDispatcher();
    testThreads();
    return 0;
}

void testThreads() {
    auto runnable = [] {
        printf("%s\n", "Runnable1 正在被执行...");
        sleep(1);
    };
    const std::string name = "LXQ";
    Threads::RunOnThread(name, runnable);
    Threads::PostOnThread(name, runnable, 100);
    sleep(1);
    Threads::ReleaseThread(name);
    sleep(1);
}

void testThreadDispatcher() {
    auto dispatcher = ThreadDispatcher::GetDefaultThreadDispatcher();

    auto runnable = [] {
        std::cout << "Runnable1 正在被执行..." << std::endl;
        sleep(1);
    };

    dispatcher->RunOnBus(runnable);
    std::cout << "-------------------1" << std::endl;

    dispatcher->PostOnBus(runnable, 1);
    std::cout << "-------------------2" << std::endl;

    dispatcher->RunOnSerial(runnable);
    std::cout << "-------------------3" << std::endl;

    dispatcher->RunOnConcurrent(runnable);
    std::cout << "-------------------4" << std::endl;

    sleep(1);
    delete dispatcher;
}
