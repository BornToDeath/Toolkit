//
// Created by lixiaoqing on 2021/7/15.
//


#include <iostream>
#include "ThreadDispatcher.h"


void testThreadDispatcher();

void sleep(int secs) {
    std::this_thread::sleep_for(std::chrono::seconds(secs));
}


int main() {
    testThreadDispatcher();
    return 0;
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
