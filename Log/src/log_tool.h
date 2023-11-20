//
// Created by lixiaoqing on 2021/5/19.
//

#ifndef LOG_LOG_TOOL_H
#define LOG_LOG_TOOL_H


/**
 * 系统头文件
 */
#include "unistd.h"

#include <iostream>
#include <memory>
#include <tuple>

/**
 * 自定义头文件
 */
#include "log.h"
#include "log_common.h"

namespace log {
namespace tool {

/**
 * 获取当前 Unix 时间戳
 * @return
 */
unsigned long long CurrentTimeMills();

/**
 * 将日志输出到控制台
 * @param level
 * @param tag
 * @param format
 * @param ...
 */
void PrintLog(LogLevel level, const char *tag, const char *format, ...);

/**
 * 获取对应日志存储策略的策略名称
 * @param strategy
 * @return
 */
std::string GetLogStrategyName(LogStrategy strategy);

/**
 * 根据日志类型获取类型名
 * @param type
 * @return
 */
std::string GetLogTypeName(LogType type);

/**
 * 判断文件是否存在
 * @param filePath 文件名，绝对路径
 * @return
 */
bool IsFileExist(const char *const &filePath);

/**
 * 获取文件长度
 * @param filePath 文件名，绝对路径
 * @return
 */
long GetFileSize(const char *const &filePath);

/**
 * 创建目录
 * @param dir
 * @return
 */
bool CreateDirIfNotExist(const char *const &dir);

/**
 * 创建多级目录
 * @param dir
 * @return
 */
bool CreateMultiLevelDir(const char *dir);

/**
 * 获取当前日期
 * @param format 日期格式化字符串，例：%Y-%m-%d %H:%M:%S
 * @return
 */
std::string GetCurrentDateTime(const char *format);

}  // namespace tool
}  // namespace log


#endif //LOG_LOG_TOOL_H
