//
// Created by lixiaoqing on 2021/6/23.
//

#include <chrono>
#include "Time/TimeUtil.h"


Timestamp TimeUtil::getCurrentTimeMills() {
    std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()
    );
    return ms.count();
}

std::string TimeUtil::getCurrentDateTime(const char *const format) {
    const int size = 64;
    char now[size];

    time_t curTimestamp;

    // 获取时间戳
    time(&curTimestamp);

    // 转换为tm结构
    tm *timeinfo = localtime(&curTimestamp);

    // 格式化为标准时间。示例：%Y-%m-%d %H:%M:%S
    strftime(now, size, format, timeinfo);

    std::string ret(now);
    return ret;
}