//
// Created by lixiaoqing on 2021/6/23.
//

#ifndef UTILS_COMMONUTIL_H
#define UTILS_COMMONUTIL_H


#include <iostream>
#include "Common/Define.h"

#define GET_NAME(obj) #obj


/**
 * 通用工具类
 */
class CommonUtil final {

public:

    /**
     * 解析 data 前四个字节所表示的整形值
     * @param data
     * @param start 待解析数据的起始位置
     * @return
     */
    static int bytes2Int(const BYTE *data, int start = 0);

    /**
     * 获取对象类型
     * @tparam T
     * @param obj
     * @return
     */
    template<typename T>
    static std::string getObjectType(T obj) {
        return GET_NAME(T);
    }

    /**
     * 获取整形值对应的枚举类型的枚举值
     * @tparam T   枚举类型
     * @param type 整形值
     * @return
     */
    template<typename T>
    static T getEnumValueOfInt(int type) {
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
    static int getIntValueOfEnum(T type) {
        auto value = static_cast<typename std::underlying_type<T>::type>(type);
        return value;
    }

    /**
     * 输出到标准输出
     * @param format
     */
    static void print(const char *format, ...);

public:

    CommonUtil() = delete;

    ~CommonUtil() = delete;

};


#endif //UTILS_COMMONUTIL_H
