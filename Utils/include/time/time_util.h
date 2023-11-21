//
// Created by lixiaoqing on 2021/6/23.
//

#ifndef UTILS_TIME_UTIL_H
#define UTILS_TIME_UTIL_H

#include <iostream>

#include "common/define.h"

namespace utils {

/**
 * 时间工具类
 */
namespace time_util {

/**
 * 获取当前 Unix 时间戳 (单位: 毫秒)
 * @return
 */
Timestamp GetCurrentTimeMills();

/**
 * 获取当前日期
 * @param format 日期格式化字符串，默认：%Y-%m-%d %H:%M:%S
 * @return
 */
std::string GetCurrentDateTime(const char *format = "%Y-%m-%d %H:%M:%S");

}  // namespace time_util
}  // namespace utils

#endif //UTILS_TIME_UTIL_H
