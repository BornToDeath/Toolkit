//
// Created by lixiaoqing on 2021/7/15.
//


#include <iostream>
#include "ThreadDispatcher.h"
#include "Threads.h"


void sleep(int secs) {
    std::this_thread::sleep_for(std::chrono::seconds(secs));
}

void testThreadDispatcher();

void testThreads();


int main() {
//    testThreadDispatcher();
    testThreads();
    return 0;
}

void testThreads() {
    auto runnable = [] {
        printf("%s\n", "Runnable1 正在被执行...");
        sleep(3);
    };
    const std::string name = "LXQ";
    Threads::runOnThread(name, runnable);
    Threads::postOnThread(name, runnable, 1000);
    sleep(10);
    Threads::releaseThread(name);
    sleep(1000000);
}

void testThreadDispatcher() {
    auto dispatcher = ThreadDispatcher::getDefaultThreadDispatcher();

    auto runnable = [] {
        std::cout << "Runnable1 正在被执行..." << std::endl;
        sleep(3);
    };

    dispatcher->runOnBus(runnable);
    std::cout << "-------------------1" << std::endl;

    dispatcher->postOnBus(runnable, 5);
    std::cout << "-------------------2" << std::endl;

    dispatcher->runOnSerial(runnable);
    std::cout << "-------------------3" << std::endl;

    dispatcher->runOnConcurrent(runnable);
    std::cout << "-------------------4" << std::endl;

    sleep(10);
    delete dispatcher;
}
