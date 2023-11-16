//
// Created by lixiaoqing on 2021/5/19.
//

#ifndef LOG_LOG_MODEL_H
#define LOG_LOG_MODEL_H


// 自定义头文件
#include "log_mmap.h"

namespace log {

/**
 * 日志模型类，每个 model 对应一种日志类型。此类不可被继承
 */
class LogModel final {

public:

    explicit LogModel(const std::string &logFileDir);

    ~LogModel() = default;

    /**
     * 禁用拷贝构造函数
     * @param model
     */
    LogModel(const LogModel &model) = delete;

    /**
     * 禁用赋值构造函数
     * @param model
     * @return
     */
    LogModel &operator=(const LogModel &model) = delete;

public:

    /**
     * 将单条日志写入文件
     * @param logData
     * @return
     */
    bool WriteLogToFile(const std::shared_ptr<LogData> &logData);

private:

    /**
     * Mmap 实现对象。进行 Mmap 映射等操作
     */
    std::shared_ptr<LogMmap> mmapImpl_;
};

}  // namespace log

#endif //LOG_LOG_MODEL_H
