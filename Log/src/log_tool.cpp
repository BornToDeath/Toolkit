//
// Created by lixiaoqing on 2021/5/19.
//

#include "log_tool.h"

/******** 系统头文件 *********/
#include <sys/prctl.h>
#include <sys/stat.h>

#include <chrono>
#include <cstdarg>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <sstream>

/** 定义日志输出到标准输出时的颜色 */
#define LOG_COLOR_RESET "\033[0m"
#define LOG_COLOR_RED "\033[31m"
#define LOG_COLOR_PURPLE "\033[35m"

namespace log {
namespace tool {

unsigned long long CurrentTimeMills() {
    std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()
    );
    return ms.count();
}

void PrintLog(LogLevel level, const char *tag, const char *format, ...) {

    if (!DEBUG) {
        return;
    }

    // 获取线程名
    char thread_name[THREAD_NAME_MAX_LEN] = {0};
    prctl(PR_GET_NAME, thread_name);

    // 日志内容
    char log_text[LOG_TEXT_MAX_LEN] = {0};

    va_list arg_list;
    va_start(arg_list, format);
    vsnprintf(log_text, LOG_TEXT_MAX_LEN, format, arg_list);
    va_end(arg_list);

    // 日志级别
    const char *log_level = logLevelName[static_cast<int>(level)];

    // 当前时间
    std::string now = GetCurrentDateTime("%Y-%m-%d %H:%M:%S");

    std::ostringstream oss;
    oss << now
        << " | " << log_level
        << " | " << tag
        << " | " << thread_name
        << " | " << log_text;

    std::string log = oss.str();
    switch (level) {
        case LogLevel::Error:
            log = LOG_COLOR_RED + log + LOG_COLOR_RESET;
            break;
        case LogLevel::Warn:
            log = LOG_COLOR_PURPLE + log + LOG_COLOR_RESET;
            break;
        default:
            break;
    }

    // 输出到控制台
    printf("%s\n", log.c_str());
}

std::string GetLogStrategyName(LogStrategy strategy) {
    switch (strategy) {
        case LogStrategy::LOG_MMAP:
            return "LOG_MMAP";
        case LogStrategy::LOG_MEMORY:
            return "LOG_MEMORY";
        case LogStrategy::LOG_MMAP_FAIL:
            return "LOG_MMAP_FAIL";
        case LogStrategy::LOG_MEMORY_FAIL:
            return "LOG_MEMORY_FAIL";
        default:
            return "LOG_NO_STRATEGY";
    }
}

std::string GetLogTypeName(LogType type) {
    switch (type) {
        case LogType::Normal:
            return "normal";
        case LogType::Error:
            return "error";
        default:
            return "unknown";
    }
}

bool IsFileExist(const char *const &filepath) {
    return 0 == access(filepath, F_OK);
}

long GetFileSize(const char *const &filepath) {

    FILE *fp = fopen(filepath, "rb");
    if (fp == nullptr) {
        return -1;
    }

    fseek(fp, 0, SEEK_END);
    long file_size = ftell(fp);
    fclose(fp);

    return file_size;
}

bool CreateDirIfNotExist(const char *const &dir) {
    if (dir == nullptr) {
        return false;
    }

    // 如果日志目录不存在则创建
    if (0 != access(dir, 0)) {
        if (0 != mkdir(dir, 0755)) {
            return false;
        }
    }
    return true;
}

bool CreateMultiLevelDir(const char *dir) {
    if (dir == nullptr) {
        return false;
    }

    std::string root = dir;
    if (root[root.length() - 1] != '/') {
        root.push_back('/');
    }

    // 先尝试创建
    if (CreateDirIfNotExist(dir)) {
        return true;
    }

    // 再递归创建
    bool isOk = false;
    unsigned long offset = 1;

    do {
        auto index = root.find('/', offset);
        if (index == std::string::npos) {
            break;
        }
        std::string temp = root.substr(0, index);
        isOk = CreateDirIfNotExist(temp.c_str());
        offset = index + 1;
    } while (isOk);

    return isOk;
}

std::string GetCurrentDateTime(const char *const format) {

    {
        /**
         * 方式一
         * 示例：2021-08-04 19:56:03
         */

//        const int size = 64;
//        char now[size];
//
//        time_t curTimestamp;
//
//        // 获取时间戳
//        time(&curTimestamp);
//
//        // 转换为tm结构
//        tm *timeinfo = localtime(&curTimestamp);
//
//        // 格式化为标准时间。示例：%Y-%m-%d %H:%M:%S
//        strftime(now, size, format, timeinfo);
//
//        std::string ret(now);
//        return ret;
    }


    {
        /**
         * 方式二：精确到毫秒
         * 示例：2021-08-04 21:51:53.443
         */

        auto now = std::chrono::system_clock::now();
        auto cur_time = std::chrono::system_clock::to_time_t(now);

        // 日期
        const int size = 64;
        char buf[size] = {0};
        strftime(buf, size, format, std::localtime(&cur_time));

        // 毫秒
        auto mills = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;

        std::ostringstream oss;
        oss << buf
            << '.'
            << std::setw(3) << std::setfill('0')  // 宽度为3，空白用0填充
            << mills.count();

        return oss.str();
    }
}

void EncryptLog(const std::string &input_log, std::string &output_log) {

    const std::string key = LOG_ENCRYPT_KEY;
    size_t key_len = key.length();

    char *base64_input = internal::Base64Encode(input_log.c_str(), input_log.length());
    int base64_len = static_cast<int>(strlen(base64_input));

    for (int i = 0; i < base64_len; i++) {
//        char c = base64Input[i];
        for (int j = 0; j < key_len; j++) {
            // 为什么这种方法加密之后的数据不对？
//            c ^= key[j];
            char s = (char) (base64_input[i] ^ key[j]);
            base64_input[i] = s;
        }
//        base64Input[i] = c;
    }

    // 注意：base64Len 的长度必须传进去，否则编码有问题！问题是：在 Base64Encode() 内部通过 strlen() 函数获取到的 base64Input 长度和此时获取到的长度不同
    char *result = internal::Base64Encode(base64_input, base64_len);

    output_log = result;

    free(base64_input);
    free(result);
}

namespace internal {

char *Base64Encode(const char *data, size_t data_len) {

    const char base[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=";
    size_t ret_len = data_len / 3;
    int prepare = 0;
    int temp = data_len % 3;
    int tmp = 0;
    int i = 0;
    char *ret = nullptr;
    char *f = nullptr;
    char changed[4];

    if (temp > 0) {
        ret_len += 1;
    }

    ret_len = ret_len * 4 + 1;
    ret = (char *) malloc(ret_len);
    if (ret == nullptr) {
        printf("No enough memory.");
        exit(0);
    }
    memset(ret, 0, ret_len);
    f = ret;

    while (tmp < data_len) {
        temp = 0;
        prepare = 0;
        memset(changed, '\0', 4);
        while (temp < 3) {
            if (tmp >= data_len) {
                break;
            }
            prepare = ((prepare << 8) | (data[tmp] & 0xFF));
            tmp++;
            temp++;
        }
        prepare = (prepare << ((3 - temp) * 8));
        for (i = 0; i < 4; i++) {
            if (temp < i) {
                changed[i] = 0x40;
            } else {
                changed[i] = (prepare >> ((3 - i) * 6)) & 0x3F;
            }
            *f = base[changed[i]];
            f++;
        }
    }
    *f = '\0';

    return ret;
}

}  // namespace internal
}  // namespace tool
}  // namespace log