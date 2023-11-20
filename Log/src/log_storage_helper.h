//
// Created by lixiaoqing on 2021/5/19.
//

#ifndef LOG_LOG_STORAGE_HELPER_H
#define LOG_LOG_STORAGE_HELPER_H


/**
 * 系统头文件
 */
#include <cstdio>
#include <string>
#include <vector>

/**
 * 自定义头文件
 */
#include "log_common.h"

namespace log {

/**
 * 日志存储器。
 * 进行日志文件存储相关的具体操作，包括文件重命名、文件删除等。此类不可被继承
 */
class LogStorageHelper final {

/* ======================================================= */
public: /* Methods                                         */
/* ======================================================= */

    /**
     *
     * @param log_file_dir 日志目录
     */
    explicit LogStorageHelper(std::string log_file_dir);

    /**
     * 析构函数
     */
    ~LogStorageHelper() = default;

    /**
     * 禁用拷贝构造
     * @param log_storage_helper
     */
    LogStorageHelper(const LogStorageHelper &log_storage_helper) = delete;

    /**
     * 禁用赋值构造
     * @param log_storage_helper
     * @return
     */
    LogStorageHelper &operator=(const LogStorageHelper &log_storage_helper) = delete;

public:

    /**
     * 准备日志文件，为 Mmap 映射做准备
     * @param file_path  日志文件名。绝对路径
     * @param log_offset 如果文件可用，则标识日志待写入的文件位置
     * @return
     */
    LogFileStatus PrepareLogFile(const char *file_path, long &log_offset);

    /**
     * 重命名日志文件
     * @return
     */
    bool RenameLogFile();

    /**
     * 删除历史日志数据。
     * @return 删除成功的历史日志文件的个数
     */
    int DeleteOldestLogFile();

    /**
     * logFileMaxIndex_ get 方法
     * @return
     */
    long GetLogFileMaxIndex() const;

    /**
     * logFileMinIndex_ get 方法
     * @return
     */
    long GetLogFileMinIndex() const;

    /**
     * logFileNums_ get 方法
     * @return
     */
    long GetLogFileNums() const;

/* ======================================================= */
private: /* Methods                                        */
/* ======================================================= */

    /**
     * 更新日志文件相关参数，包括：
     *   1. 历史日志文件名最大索引
     *   2. 历史日志文件名最小索引
     *   3. 历史日志文件的个数
     * @return
     */
    bool UpdateLogFileParams();

    /**
     * 获取文件第一个 \0 出现的位置，作为将来日志的存储开始位置
     * @param file 文件指针
     * @return
     */
    long GetLocationToBeWritten(FILE *file) const;


/* ======================================================= */
private: /* Fields                                         */
/* ======================================================= */

    /**
     * 日志目录
     */
    const std::string log_file_dir_;

    /**
     * 历史日志文件名最大索引
     */
    long log_file_max_index_;

    /**
     * 历史日志文件名最小索引
     */
    long log_file_min_index_;

    /**
     * 当前历史日志文件的个数
     */
    long log_file_nums_;

};

}  // namespace log

#endif //LOG_LOG_STORAGE_HELPER_H
