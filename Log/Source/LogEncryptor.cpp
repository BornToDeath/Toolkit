//
// Created by lixiaoqing on 2021/5/19.
//

/** 系统头文件  */
#include <cstring>

/**
 * 自定义头文件
 */
#include "LogEncryptor.h"
#include "LogCommon.h"
#include "LogTools.h"

namespace log {

void LogEncryptor::EncryptLog(const std::string &inputLog, std::string &outputLog) {

    if (DEBUG) {
        outputLog = inputLog;
        return;
    }

    const std::string key = LOG_ENCRYPT_KEY;
    size_t keyLen = key.length();

    char *base64Input = Base64Encode(inputLog.c_str(), inputLog.length());
    int base64Len = static_cast<int>(strlen(base64Input));

    for (int i = 0; i < base64Len; i++) {
//        char c = base64Input[i];
        for (int j = 0; j < keyLen; j++) {
            // 为什么这种方法加密之后的数据不对？
//            c ^= key[j];
            char s = (char) (base64Input[i] ^ key[j]);
            base64Input[i] = s;
        }
//        base64Input[i] = c;
    }

    // 注意：base64Len 的长度必须传进去，否则编码有问题！问题是：在 Base64Encode() 内部通过 strlen() 函数获取到的 base64Input 长度和此时获取到的长度不同
    char *result = Base64Encode(base64Input, base64Len);

    outputLog = result;

    free(base64Input);
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