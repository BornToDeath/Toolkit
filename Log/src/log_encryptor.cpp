//
// Created by lixiaoqing on 2021/5/19.
//

#include "log_encryptor.h"

/** 系统头文件  */
#include <cstring>

/**
 * 自定义头文件
 */
#include "log_common.h"
#include "log_tool.h"

namespace log {

void LogEncryptor::EncryptLog(const std::string &input_log, std::string &output_log) {

    if (DEBUG) {
        output_log = input_log;
        return;
    }

    const std::string key = LOG_ENCRYPT_KEY;
    size_t key_len = key.length();

    char *base64_input = Base64Encode(input_log.c_str(), input_log.length());
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
    char *result = Base64Encode(base64_input, base64_len);

    output_log = result;

    free(base64_input);
    free(result);
}

char *LogEncryptor::Base64Encode(const char *data, size_t data_len) {

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

}  // namespace log