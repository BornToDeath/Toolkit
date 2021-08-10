//
// Created by lixiaoqing on 2021/5/19.
//

/********* 系统头文件 **********/
#include <memory>

/********* 自定义头文件 **********/
#include "LogModel.h"
#include "LogTools.h"

/********* 宏定义 **********/
#define TAG "LogModel"


LogModel::LogModel(const std::string &logFileDir) : mmapImpl(nullptr) {
    // 进行 mmap 映射
    this->mmapImpl = std::make_shared<LogMmap>(logFileDir);
}

bool LogModel::writeLogToFile(const std::shared_ptr<LogData> &logData) {

    if (LogStrategy::LOG_MMAP == this->mmapImpl->getLogStrategy()) {

        // 如果是 mmap 机制
        this->mmapImpl->writeLogToFileInMmap(logData);

    } else if (LogStrategy::LOG_MEMORY == this->mmapImpl->getLogStrategy()) {

        // 如果是内存缓存机制
        this->mmapImpl->writeLogToFileInMem(logData);

    } else {
        if (DEBUG) {
            LogTools::printLog(LogLevel::Error, TAG, LOG_THREAD_NAME,
                               ">>> 无法存储日志，请重新初始化日志存储策略！");
        }
        return false;
    }
    return true;
}

