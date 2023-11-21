//
// Created by lixiaoqing on 2021/6/23.
//

#ifndef UTILS_COMMON_UTIL_H
#define UTILS_COMMON_UTIL_H


#include <iostream>

#include "common/define.h"

namespace utils {

/**
 * 通用工具类
 */
namespace common_util {

/**
 * 解析 data 前四个字节所表示的整形值
 * @param data
 * @param start 待解析数据的起始位置
 * @return
 */
int Bytes2Int(const BYTE *data, int start = 0);

/**
 * 获取整形值对应的枚举类型的枚举值
 * @tparam T   枚举类型
 * @param type 整形值
 * @return
 */
template<typename T>
T GetEnumValueOfInt(int type) {
    auto value = T(type);
    return value;
}

/**
 * 获取枚举类型的枚举值对应的整形值
 * @tparam T   枚举类型
 * @param type 枚举值
 * @return
 */
template<typename T>
int GetIntValueOfEnum(T type) {
    auto value = static_cast<typename std::underlying_type<T>::type>(type);
    return value;
}

/**
 * 输出到标准输出
 * @param format
 */
void Print(const char *format, ...);

}  // namespace common_util
}  // namespace utils

#endif //UTILS_COMMON_UTIL_H
