//
// Created by lixiaoqing on 2021/6/23.
//

#ifndef UTILS_TIMEUTIL_H
#define UTILS_TIMEUTIL_H


#include <iostream>
#include "Common/Define.h"


/**
 * 时间工具类
 */
class TimeUtil final {

public:

    /**
     * 获取当前 Unix 时间戳 (单位: 毫秒)
     * @return
     */
    static Timestamp getCurrentTimeMills();

    /**
     * 获取当前日期
     * @param format 日期格式化字符串，默认：%Y-%m-%d %H:%M:%S
     * @return
     */
    static std::string getCurrentDateTime(const char *format = "%Y-%m-%d %H:%M:%S");


public:

    TimeUtil() = delete;

    ~TimeUtil() = delete;

};


#endif //UTILS_TIMEUTIL_H
