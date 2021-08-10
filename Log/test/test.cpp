//
// Created by lixiaoqing on 2021/8/8.
//

#include <thread>
#include "Log.h"

#define TAG "Test"


int main() {

    Log::init("/home/lixiaoqing/test/logs/");

    const int count = 10000;
    for (int i = 1; i <= count; ++i) {
        Log::debug(TAG, "%d | %s", count, "「斯人若彩虹，遇上方知有。」");
        Log::info(TAG, "%d | %s", count, "「斯人若彩虹，遇上方知有。」");
        Log::warn(TAG, "%d | %s", count, "「斯人若彩虹，遇上方知有。」");
        Log::error(TAG, "%d | %s", count, "「斯人若彩虹，遇上方知有。」");
    }

    Log::info(TAG, "进程退出");
    std::this_thread::sleep_for(std::chrono::seconds(1));

    return 0;
}