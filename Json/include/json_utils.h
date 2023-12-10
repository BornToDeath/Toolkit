
#ifndef JSON_JSON_UTILS_H
#define JSON_JSON_UTILS_H

#include <fstream>
#include <sstream>

#include "json/json.h"


/**
 * 封装的 Json 工具类
 */
namespace json_util {

/**
 * 将一个 Json 字符串转为 Json::Value.
 */
bool String2JsonValue(Json::Value &outValue, const std::string &json) {
    Json::Reader reader;
    return reader.parse(json, outValue, false);
}

/**
 * 判断 obj 中是否包含指定 name 的元素。
 */
inline bool Has(const Json::Value &obj, const std::string &name) {
    return obj.isMember(name);
}

/**
 * 读取一个 int 元素。
 * 如果不存在，返回 defValue 。
 * 如果 name 对应的字段不是一个 int 类型，且无法自动转换，返回 defValue 。
 */
int GetInt(const Json::Value &obj, const std::string &name, int defValue) {
    if (!Has(obj, name)) {
        return defValue;
    }

    Json::Value temp = obj.get(name, defValue);
    if (temp.isNumeric()) {
        return temp.asInt();
    }

    return defValue;
}

/**
 * 读取一个 bool 元素。
 * 如果不存在，返回 default_value 。
 * 如果 name 对应的字段不是一个 bool 类型，且无法自动转换，返回 default_value 。
 */
bool GetBool(const Json::Value &obj, const std::string &name, bool default_value) {
    if (!Has(obj, name)) {
        return default_value;
    }

    Json::Value temp = obj.get(name, default_value);
    if (temp.isBool()) {
        return temp.asBool();
    }

    // 如果是 String， 尝试自动转换
    if (temp.isString()) {
        std::string s = temp.asString();
        if (s == "0") {
            return false;
        }
        if (s == "1") {
            return true;
        }
    }
        // 如果是 数字
    else if (temp.isNumeric()) {
        int i = temp.asInt();
        if (i == 0) {
            return false;
        }
        if (i == 1) {
            return true;
        }
    }

    return default_value;
}

/**
 * 读取一个 double 元素。
 * 如果不存在，返回 default_value 。
 * 如果 name 对应的字段不是一个 double 类型，且无法自动转换，返回 default_value 。
 */
double GetDouble(const Json::Value &obj, const std::string &name, double default_value) {
    if (!Has(obj, name)) {
        return default_value;
    }

    Json::Value temp = obj.get(name, default_value);
    if (temp.isNumeric()) {
        return temp.asDouble();
    }

    return default_value;
}

/**
 * 读取一个 string 元素。
 * 如果不存在，返回 default_value 。
 * 如果 name 对应的字段不是一个 string 类型，且无法自动转换，返回 default_value 。
 */
std::string GetString(const Json::Value &obj, const std::string &name, std::string default_value) {
    if (!Has(obj, name)) {
        return default_value;
    }

    Json::Value temp = obj.get(name, default_value);
    if (temp.isString()) {
        return temp.asString();
    }

    if (temp.isNumeric()) {
        std::ostringstream oss;
        if (temp.isInt()) {
            oss << temp.asInt();
        } else if (temp.isBool()) {
            oss << (temp.asBool() ? "true" : "false");
        } else if (temp.isUInt()) {
            oss << temp.asUInt();
        } else if (temp.isDouble()) {
            oss << temp.asDouble();
        } else {
            return default_value;
        }

        std::string s = oss.str();
        return s;
    }

    return default_value;
}

/**
 * 读取一个 unsigned int 元素。
 * 如果不存在，返回 default_value 。
 * 如果 name 对应的字段不是一个 unsigned int 类型，且无法自动转换，返回 default_value 。
 */
unsigned int GetUnsignedInt(const Json::Value &obj, const std::string &name, unsigned int default_value) {
    if (!Has(obj, name)) {
        return default_value;
    }

    Json::Value temp = obj.get(name, default_value);
    if (temp.isNumeric()) {
        return temp.asUInt();
    }

    return default_value;
}

unsigned long long GetULongLong(const Json::Value &obj, const std::string &name, unsigned long long default_value) {
    if (!Has(obj, name)) {
        return default_value;
    }

    Json::Value temp = obj.get(name, default_value);
    if (temp.isNumeric()) {
        return temp.asULongLong();
    }

    return default_value;
}

long long GetLongLong(const Json::Value &obj, const std::string &name, long long default_value) {
    if (!Has(obj, name)) {
        return default_value;
    }

    Json::Value temp = obj.get(name, default_value);
    if (temp.isNumeric()) {
        return temp.asLongLong();
    }

    return default_value;
}

/**
 * 如果 obj 中的元素是一个 Json::Value 格式的数据，则可通过此方法获取该数据
 * @param obj
 * @param name
 * @return
 */
Json::Value GetJsonValue(const Json::Value &obj, const std::string &name) {
    Json::Value default_value = Json::nullValue;
    if (!Has(obj, name)) {
        return default_value;
    }

    default_value = obj.get(name, default_value);
    return default_value;
}

/**
 * 从一个 Json 文件中读取内容并转换为 Json::Value 格式
 * @param filepath
 * @param obj
 * @return
 */
bool ReadJsonFile(const std::string &filepath, Json::Value &obj) {
    std::ifstream ifs(filepath);
    if (!ifs.is_open()) {
        return false;
    }
    std::string content((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
    ifs.close();
    if (content.empty()) {
        return false;
    }

    return String2JsonValue(obj, content);
}

/**
 * Json::Value 转 string
 * @param obj
 * @return
 */
std::string JsonValue2String(const Json::Value &obj) {
    return Json::FastWriter().write(obj);
}

};

#endif //JSON_JSON_UTILS_H
