//
// Created by lixiaoqing on 2021/5/19.
//

#ifndef LOG_LOGMODEL_H
#define LOG_LOGMODEL_H


// 自定义头文件
#include "LogMmap.h"


/**
 * 日志模型类，每个 model 对应一种日志类型。此类不可被继承
 */
class LogModel final {

public:

    explicit LogModel(const std::string &logFileDir);

    ~LogModel() = default;

    /**
     * 将单条日志写入文件
     * @param logData
     * @return
     */
    bool writeLogToFile(const std::shared_ptr<LogData> &logData);

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

private:

    /**
     * mmap 实现对象。进行 mmap 映射等操作
     */
    std::shared_ptr<LogMmap> mmapImpl;
};


#endif //LOG_LOGMODEL_H
