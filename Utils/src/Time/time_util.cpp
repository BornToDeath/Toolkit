//
// Created by lixiaoqing on 2021/6/23.
//

#include "time/time_util.h"

#include <chrono>

namespace utils {
namespace time_util {

Timestamp GetCurrentTimeMills() {
    std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()
    );
    return ms.count();
}

std::string GetCurrentDateTime(const char *const format) {
    const int size = 64;
    char now[size];

    time_t timestamp;

    // 获取时间戳
    time(&timestamp);

    // 转换为tm结构
    tm *timeinfo = localtime(&timestamp);

    // 格式化为标准时间。示例：%Y-%m-%d %H:%M:%S
    strftime(now, size, format, timeinfo);

    std::string ret(now);
    return ret;
}

}  // namespace time_util
}  // namespace utils