//
// Created by lixiaoqing on 2021/6/24.
//

#include "string/string_util.h"

#include <cstring>
#include <sstream>

#include "md5.h"

namespace utils {
namespace string_util {

using namespace md5;

bool Compare(const std::string &s1, const std::string &s2) {
    return (s1 == s2);
}

bool Compare(const char *s1, const char *s2) {
    return (strcmp(s1, s2) == 0);
}

int String2Int(const std::string &obj) {
    int res;
    try {
        res = std::stoi(obj);
    } catch (...) {
        res = 0;
    };
    return res;
}

void Trim(std::string &str) {
    if (str.empty()) {
        return;
    }
    str.erase(0, str.find_first_not_of(' '));
    str.erase(str.find_last_not_of(' ') + 1);
}

std::string GetMd5(const std::string &str) {
    if (str.empty()) {
        return "";
    }
    MD5 md5;
    md5.update(str);
    return md5.toString();
}

std::vector<std::string> Split(const std::string &str, const char sep) {
    if (str.empty()) {
        return {};
    }
    std::vector<std::string> ans;
    std::istringstream iss(str);
    std::string word;
    while (getline(iss, word, sep)) {
        if (!word.empty()) {
            ans.emplace_back(word);
        }
    }
    return ans;
}

}  // namespace string_util
}  // namespace utils