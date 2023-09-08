//
// Created by lixiaoqing on 2021/6/24.
//

#ifndef UTILS_STRINGUTIL_H
#define UTILS_STRINGUTIL_H

#include <iostream>
#include <vector>

/**
 * 字符串工具类
 */
namespace StringUtil {

    /**
     * 判断两个字符串是否相等
     * @param s1
     * @param s1
     * @return
     */
    bool compare(const std::string &s1, const std::string &s2);

    /**
     * 判断两个字符数组是否相等
     * @param s1
     * @param s2
     * @return
     */
    bool compare(const char *s1, const char *s2);

    /**
     * string 转 int
     * @param obj
     * @return 失败返回 0
     */
    int string2Int(const std::string &obj);

    /**
     * 去除首尾空格
     * @param str
     */
    void trim(std::string &str);

    /**
     * 获取字符串的 MD5 值
     * @param str
     */
    std::string getMD5(const std::string &str);

    /**
     * 按固定字符分割字符串
     * @param str
     * @param sep
     * @return
     */
    std::vector<std::string> split(const std::string &str, const char sep);

};

#endif //UTILS_STRINGUTIL_H
