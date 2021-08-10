//
// Created by lixiaoqing on 2021/5/19.
//

#ifndef LOG_LOGSTORAGEHELPER_H
#define LOG_LOGSTORAGEHELPER_H


/**
 * 系统头文件
 */
#include <string>
#include <vector>
#include <cstdio>

/**
 * 自定义头文件
 */
#include "LogCommon.h"


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
     * @param logFileDir 日志目录
     */
    explicit LogStorageHelper(std::string  logFileDir);

    /**
     * 析构函数
     */
    ~LogStorageHelper() = default;

    /**
     * 准备日志文件，为 mmap 映射做准备
     * @param filePath  日志文件名。绝对路径
     * @param logOffset 如果文件可用，则标识日志待写入的文件位置
     * @return
     */
    LogFileStatus prepareLogFile(const char *filePath, long &logOffset);

    /**
     * 重命名日志文件
     * @return
     */
    bool renameLogFile();

    /**
     * 删除历史日志数据。
     * @return 删除成功的历史日志文件的个数
     */
    int deleteOldestLogFile();

    /**
     * logFileMaxIndex get 方法
     * @return
     */
    long getLogFileMaxIndex() const;

    /**
     * logFileMinIndex get 方法
     * @return
     */
    long getLogFileMinIndex() const;

    /**
     * logFileNums get 方法
     * @return
     */
    long getLogFileNums() const;

    /**
     * 禁用拷贝构造
     * @param logStorageHelper
     */
    LogStorageHelper(const LogStorageHelper &logStorageHelper) = delete;

    /**
     * 禁用赋值构造
     * @param logStorageHelper
     * @return
     */
    LogStorageHelper &operator=(const LogStorageHelper &logStorageHelper) = delete;

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
    bool updateLogFileParams();

    /**
     * 获取文件第一个 \0 出现的位置，作为将来日志的存储开始位置
     * @param file 文件指针
     * @return
     */
    long getLocationToBeWritten(FILE *file) const;


/* ======================================================= */
private: /* Fields                                         */
/* ======================================================= */

    /**
     * 日志目录
     */
    const std::string logFileDir;

    /**
     * 历史日志文件名最大索引
     */
    long logFileMaxIndex;

    /**
     * 历史日志文件名最小索引
     */
    long logFileMinIndex;

    /**
     * 当前历史日志文件的个数
     */
    long logFileNums;

};

#endif //LOG_LOGSTORAGEHELPER_H
