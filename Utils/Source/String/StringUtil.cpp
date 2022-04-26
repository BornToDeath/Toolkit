//
// Created by lixiaoqing on 2021/6/24.
//

#include <cstring>
#include "String/StringUtil.h"
#include "MD5.h"


bool StringUtil::compare(const std::string &s1, const std::string &s2) {
    return (s1 == s2);
}

bool StringUtil::compare(const char *s1, const char *s2) {
    return (strcmp(s1, s2) == 0);
}

int StringUtil::string2Int(const std::string &obj) {
    int res;
    try {
        res = std::stoi(obj);
    } catch (...) {
        res = 0;
    };
    return res;
}

void StringUtil::trim(std::string &str) {
    if (str.empty()) {
        return;
    }
    str.erase(0, str.find_first_not_of(' '));
    str.erase(str.find_last_not_of(' ') + 1);
}

std::string StringUtil::getMD5(const std::string &str) {
    if (str.empty()) {
        return "";
    }
    MD5 md5;
    md5.update(str);
    return md5.toString();
}