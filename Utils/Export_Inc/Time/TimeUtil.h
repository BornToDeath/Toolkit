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
namespace TimeUtil {

    /**
     * 获取当前 Unix 时间戳 (单位: 毫秒)
     * @return
     */
    Timestamp getCurrentTimeMills();

    /**
     * 获取当前日期
     * @param format 日期格式化字符串，默认：%Y-%m-%d %H:%M:%S
     * @return
     */
    std::string getCurrentDateTime(const char *format = "%Y-%m-%d %H:%M:%S");

};


#endif //UTILS_TIMEUTIL_H
