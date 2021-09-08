//
// Created by lixiaoqing on 2021/9/3.
//

#include <iostream>
#include <thread>
#include "SQLiteDatabase.h"


void sleep(int seconds);

template<class Type>
void logger(const Type text) {
    auto curTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    char buf[64] = {0};
    strftime(buf, 64, "%Y-%m-%d %H:%M:%S", std::localtime(&curTime));
    std::cout << buf << " | " << std::this_thread::get_id() << " | " << text << std::endl;
}

/** 主函数 */
int main() {
    logger("===== 开始测试 =====");

    SQLiteDatabase db;
    logger(db.isThreadSafe());

    logger("===== 结束测试 =====");
    sleep(100000);
    return 0;
}

void sleep(int seconds) {
    std::this_thread::sleep_for(std::chrono::seconds(seconds));
}