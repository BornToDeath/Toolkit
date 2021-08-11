//
// Created by lixiaoqing on 2021/8/8.
//

#include <thread>
#include "Log.h"

#define TAG "Test"


int main() {

    Log::init("/home/lixiaoqing/test/logs/");

    const int count = 100;
    for (int i = 1; i <= count; ++i) {
        Log::debug(TAG, "%d | %s", i, "「斯人若彩虹，遇上方知有。」");
        Log::info(TAG, "%d | %s", i, "「斯人若彩虹，遇上方知有。」");
        Log::warn(TAG, "%d | %s", i, "「斯人若彩虹，遇上方知有。」");
        Log::error(TAG, "%d | %s", i, "「斯人若彩虹，遇上方知有。」");
    }

    Log::info(TAG, "进程退出");
    Log::release();

    return 0;
}