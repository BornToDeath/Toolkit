//
// Created by lixiaoqing on 2021/5/19.
//

#ifndef LOG_LOGTOOLS_H
#define LOG_LOGTOOLS_H


/**
 * 系统头文件
 */
#include <iostream>
#include <tuple>
#include <memory>
#include "unistd.h"

/**
 * 自定义头文件
 */
#include "LogCommon.h"
#include "Log.h"


using TuplePtr = std::shared_ptr<std::tuple<BYTE *, long>>;


namespace LogTools {

    /**
     * 获取当前 Unix 时间戳
     * @return
     */
    unsigned long long currentTimeMills();

    /**
     * 将日志输出到控制台
     * @param level
     * @param tag
     * @param threadName
     * @param format
     * @param ...
     */
    void printLog(LogLevel level, const char *tag, const char *threadName, const char *format, ...);

    /**
     * 获取对应日志存储策略的策略名称
     * @param strategy
     * @return
     */
    std::string getLogStrategyName(LogStrategy strategy);

    /**
     * 根据日志类型获取类型名
     * @param type
     * @return
     */
    std::string getLogTypeName(LogType type);

    /**
     * 判断文件是否存在
     * @param filePath 文件名，绝对路径
     * @return
     */
    bool isFileExist(const char *const &filePath);

    /**
     * 获取文件长度
     * @param filePath 文件名，绝对路径
     * @return
     */
    long getFileSize(const char *const &filePath);

    /**
     * 创建目录
     * @param dir
     * @return
     */
    bool createDirIfNotExist(const char *const &dir);

    /**
     * 创建多级目录
     * @param dir
     * @return
     */
    bool createMultiLevelDir(const char *dir);

    /**
     * 获取当前日期
     * @param format 日期格式化字符串，例：%Y-%m-%d %H:%M:%S
     * @return
     */
    std::string getCurrentDateTime(const char *format);

}


#endif //LOG_LOGTOOLS_H
