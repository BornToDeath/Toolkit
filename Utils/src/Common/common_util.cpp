//
// Created by lixiaoqing on 2021/6/23.
//

#include "common/common_util.h"

#include <cstring>
#include <cstdarg>

#include "time/time_util.h"

namespace utils {

namespace common_util {

int Bytes2Int(const BYTE *data, int start) {
    if (data == nullptr || start < 0) {
        return 0;
    }

    // 方式 1:
    int type = 0;
    memcpy(&type, data + start, 4);
    return type;

    // 方式 2:
//    if (data == nullptr || start < 0) {
//        return 0;
//    }
//    int len = data[start + 0] & 0xFF;
//    len |= ((data[start + 1] << 8) & 0xFF00);
//    len |= ((data[start + 2] << 16) & 0xFF0000);
//    len |= ((data[start + 3] << 24) & 0xFF000000);
//    return len;
}

void Print(const char *format, ...) {
    char log[1024] = {0};
    va_list args;
    va_start(args, format);
    vsnprintf(log, sizeof(log), format, args);
    va_end(args);

    const std::string now = time_util::GetCurrentDateTime("%Y-%m-%d %H:%M:%S");
    printf("%s | %s\n", now.c_str(), log);
}

}  // namespace common_util
}  // namespace utils