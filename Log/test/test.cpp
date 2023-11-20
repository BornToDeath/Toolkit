//
// Created by lixiaoqing on 2021/8/8.
//

#include <thread>

#include "log.h"

#define TAG "Test"


int main() {

    Log::Init("/home/tmp/logs/");

    auto runnable = []() {
        const int count = 10000;
        for (int i = 1; i <= count; ++i) {
            Log::Debug(TAG, "%d | %s", i, "「斯人若彩虹，遇上方知有。」");
            Log::Info(TAG, "%d | %s", i, "「斯人若彩虹，遇上方知有。」");
            Log::Warn(TAG, "%d | %s", i, "「斯人若彩虹，遇上方知有。」");
            Log::Error(TAG, "%d | %s", i, "「斯人若彩虹，遇上方知有。」");
        }
    };
    std::thread(runnable).detach();

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    Log::Info(TAG, "进程退出");
    Log::Stop();

    std::this_thread::sleep_for(std::chrono::hours(1));
    return 0;
}