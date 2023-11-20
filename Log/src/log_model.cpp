//
// Created by lixiaoqing on 2021/5/19.
//

#include "log_model.h"

/********* 系统头文件 **********/
#include <memory>

/********* 自定义头文件 **********/
#include "log_tool.h"

/********* 宏定义 **********/
#define TAG "LogModel"

namespace log {

LogModel::LogModel(const std::string &log_file_dir) : mmap_impl_(nullptr) {
    // 进行 Mmap 映射
    this->mmap_impl_ = std::make_shared<LogMmap>(log_file_dir);
}

bool LogModel::WriteLogToFile(const std::shared_ptr<LogData> &log_data) {

    if (LogStrategy::LOG_MMAP == this->mmap_impl_->GetLogStrategy()) {

        // 如果是 Mmap 机制
        this->mmap_impl_->WriteLogToFileInMmap(log_data);

    } else if (LogStrategy::LOG_MEMORY == this->mmap_impl_->GetLogStrategy()) {

        // 如果是内存缓存机制
        this->mmap_impl_->WriteLogToFileInMem(log_data);

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