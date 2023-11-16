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

namespace log {

LogModel::LogModel(const std::string &logFileDir) : mmapImpl_(nullptr) {
    // 进行 Mmap 映射
    this->mmapImpl_ = std::make_shared<LogMmap>(logFileDir);
}

bool LogModel::WriteLogToFile(const std::shared_ptr<LogData> &logData) {

    if (LogStrategy::LOG_MMAP == this->mmapImpl_->GetLogStrategy()) {

        // 如果是 Mmap 机制
        this->mmapImpl_->WriteLogToFileInMmap(logData);

    } else if (LogStrategy::LOG_MEMORY == this->mmapImpl_->GetLogStrategy()) {

        // 如果是内存缓存机制
        this->mmapImpl_->WriteLogToFileInMem(logData);

    } else {
        if (DEBUG) {
            tool::PrintLog(LogLevel::Error, TAG, LOG_THREAD_NAME,
                           ">>> 无法存储日志，请重新初始化日志存储策略！");
        }
        return false;
    }
    return true;
}

}  // namespace log