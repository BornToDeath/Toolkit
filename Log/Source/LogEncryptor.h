//
// Created by lixiaoqing on 2021/5/19.
//

#ifndef LOG_LOGENCRYPTOR_H
#define LOG_LOGENCRYPTOR_H


/**
 * 系统头文件
 */
#include <iostream>
#include <string>

namespace log {

/**
 * 日志加密类。
 * 对日志内容进行加密。该类不可被继承
 */
class LogEncryptor final {

/* ======================================================= */
public: /* Methods                                         */
/* ======================================================= */

    /**
     * 对日志进行加密
     * @param inputLog  加密前日志
     * @param outputLog 加密后日志
     */
    static void EncryptLog(const std::string &inputLog, std::string &outputLog);

private:

    static char *Base64Encode(const char *data, size_t data_len);
};

}  // namespace log

#endif //LOG_LOGENCRYPTOR_H
