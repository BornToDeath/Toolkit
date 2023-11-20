//
// Created by lixiaoqing on 2021/5/19.
//

#ifndef LOG_LOG_ENCRYPTOR_H
#define LOG_LOG_ENCRYPTOR_H


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
     * @param input_log  加密前日志
     * @param output_log 加密后日志
     */
    static void EncryptLog(const std::string &input_log, std::string &output_log);

private:

    static char *Base64Encode(const char *data, size_t data_len);
};

}  // namespace log

#endif //LOG_LOG_ENCRYPTOR_H
