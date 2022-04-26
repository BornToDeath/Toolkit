//
// Created by lixiaoqing on 2021/6/24.
//

#ifndef UTILS_STRINGUTIL_H
#define UTILS_STRINGUTIL_H

#include <iostream>


/**
 * 字符串工具类
 */
class StringUtil final {

public:

    /**
     * 判断两个字符串是否相等
     * @param s1
     * @param s1
     * @return
     */
    static bool compare(const std::string &s1, const std::string &s2);

    /**
     * 判断两个字符数组是否相等
     * @param s1
     * @param s2
     * @return
     */
    static bool compare(const char *s1, const char *s2);

    /**
     * string 转 int
     * @param obj
     * @return 失败返回 0
     */
    static int string2Int(const std::string &obj);

    /**
     * 去除首尾空格
     * @param str
     */
    static void trim(std::string &str);

    /**
     * 获取字符串的 MD5 值
     * @param str
     */
    static std::string getMD5(const std::string &str);

public:

    StringUtil() = delete;

    ~StringUtil() = delete;

};

#endif //UTILS_STRINGUTIL_H
