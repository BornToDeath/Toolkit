//
// Created by lixiaoqing on 2021/9/3.
//

#include <iostream>
#include <thread>

#include "notification_center.h"


void sleep(int seconds);


int main() {

    std::cout << "===== 开始测试 =====" << std::endl;

    auto notification = NotificationCenter::default_center_;

    notification->AddObserver("LXQ", [](void *data) {
        std::cout << "[1]消息正在被执行" << std::endl;
        sleep(1);
    });

    auto observer = [](void *data) {
        std::cout << "[2]消息正在被执行" << std::endl;
        sleep(1);
    };
    notification->AddObserver("LXQ", observer);

    notification->PostNotification("LXQ", nullptr);

    notification->RemoveObserver("LXQ", observer);

    std::cout << "===== 结束测试 =====" << std::endl;
    sleep(1);
    return 0;
}

void sleep(int seconds) {
    std::this_thread::sleep_for(std::chrono::seconds(seconds));
}