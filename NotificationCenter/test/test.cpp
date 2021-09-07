//
// Created by lixiaoqing on 2021/9/3.
//

#include <iostream>
#include <thread>
#include "NotificationCenter.h"


void sleep(int seconds);


int main() {

    std::cout << "===== 开始测试 =====" << std::endl;

    auto notification = NotificationCenter::defaultCenter;

    notification->addObserver("LXQ", [](void *data) {
        std::cout << "[1]消息正在被执行" << std::endl;
        sleep(3);
    });

    notification->addObserver("LXQ", [](void *data) {
        std::cout << "[2]消息正在被执行" << std::endl;
        sleep(5);
    });

    notification->postNotification("LXQ", nullptr);

    std::cout << "===== 结束测试 =====" << std::endl;
    sleep(100000);
    return 0;
}

void sleep(int seconds) {
    std::this_thread::sleep_for(std::chrono::seconds(seconds));
}