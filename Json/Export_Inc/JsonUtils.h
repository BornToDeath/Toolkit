
#ifndef JSON_JSONUTILS_H
#define JSON_JSONUTILS_H

#include <sstream>
#include <fstream>
#include "json/json.h"


/**
 * 封装的 Json 工具类
 */
namespace JsonUtils {

    /**
     * 将一个 Json 字符串转为 Json::Value.
     */
    bool string2JsonValue(Json::Value &outValue, const std::string &json) {
        Json::Reader reader;
        return reader.parse(json, outValue, false);
    }

    /**
     * 判断 obj 中是否包含指定 name 的元素。
     */
    inline bool has(const Json::Value &obj, const std::string &name) {
        return obj.isMember(name);
    }

    /**
     * 读取一个 int 元素。
     * 如果不存在，返回 defValue 。
     * 如果 name 对应的字段不是一个 int 类型，且无法自动转换，返回 defValue 。
     */
    int getInt(const Json::Value &obj, const std::string &name, int defValue) {
        if (!has(obj, name)) {
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
     * 如果不存在，返回 defValue 。
     * 如果 name 对应的字段不是一个 bool 类型，且无法自动转换，返回 defValue 。
     */
    bool getBool(const Json::Value &obj, const std::string &name, bool defValue) {
        if (!has(obj, name)) {
            return defValue;
        }

        Json::Value temp = obj.get(name, defValue);
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

        return defValue;
    }

    /**
     * 读取一个 double 元素。
     * 如果不存在，返回 defValue 。
     * 如果 name 对应的字段不是一个 double 类型，且无法自动转换，返回 defValue 。
     */
    double getDouble(const Json::Value &obj, const std::string &name, double defValue) {
        if (!has(obj, name)) {
            return defValue;
        }

        Json::Value temp = obj.get(name, defValue);
        if (temp.isNumeric()) {
            return temp.asDouble();
        }

        return defValue;
    }

    /**
     * 读取一个 string 元素。
     * 如果不存在，返回 defValue 。
     * 如果 name 对应的字段不是一个 string 类型，且无法自动转换，返回 defValue 。
     */
    std::string getString(const Json::Value &obj, const std::string &name, std::string defValue) {
        if (!has(obj, name)) {
            return defValue;
        }

        Json::Value temp = obj.get(name, defValue);
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
                return defValue;
            }

            std::string s = oss.str();
            return s;
        }

        return defValue;
    }

    /**
     * 读取一个 unsigned int 元素。
     * 如果不存在，返回 defValue 。
     * 如果 name 对应的字段不是一个 unsigned int 类型，且无法自动转换，返回 defValue 。
     */
    unsigned int getUnsignedInt(const Json::Value &obj, const std::string &name, unsigned int defValue) {
        if (!has(obj, name)) {
            return defValue;
        }

        Json::Value temp = obj.get(name, defValue);
        if (temp.isNumeric()) {
            return temp.asUInt();
        }

        return defValue;
    }

    unsigned long long getULongLong(const Json::Value &obj, const std::string &name, unsigned long long defValue) {
        if (!has(obj, name)) {
            return defValue;
        }

        Json::Value temp = obj.get(name, defValue);
        if (temp.isNumeric()) {
            return temp.asULongLong();
        }

        return defValue;
    }

    long long getLongLong(const Json::Value &obj, const std::string &name, long long defValue) {
        if (!has(obj, name)) {
            return defValue;
        }

        Json::Value temp = obj.get(name, defValue);
        if (temp.isNumeric()) {
            return temp.asLongLong();
        }

        return defValue;
    }

    /**
     * 如果 obj 中的元素是一个 Json::Value 格式的数据，则可通过此方法获取该数据
     * @param obj
     * @param name
     * @return
     */
    Json::Value getJsonValue(const Json::Value &obj, const std::string &name) {
        Json::Value defValue = Json::nullValue;
        if (!has(obj, name)) {
            return defValue;
        }

        defValue = obj.get(name, defValue);
        return defValue;
    }

    /**
     * 从一个 Json 文件中读取内容并转换为 Json::Value 格式
     * @param filePath
     * @param obj
     * @return
     */
    bool readJsonFile(const std::string &filePath, Json::Value &obj) {
        std::ifstream ifs(filePath);
        if (!ifs.is_open()) {
            return false;
        }
        std::string content((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
        ifs.close();
        if (content.empty()) {
            return false;
        }

        return string2JsonValue(obj, content);
    }

    /**
     * Json::Value 转 string
     * @param obj
     * @return
     */
    std::string jsonValue2String(const Json::Value &obj) {
        return Json::FastWriter().write(obj);
    }

};

#endif //JSON_JSONUTILS_H
