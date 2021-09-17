#ifndef JSON_JSONBUILDER_H
#define JSON_JSONBUILDER_H

#include <string>
#include "json/json.h"


/**
 * Json 的构造器
 */
class JsonBuilder {

public:

    JsonBuilder() = default;

    ~JsonBuilder() = default;

public:

    Json::Value &get() {
        return this->obj;
    }

    JsonBuilder &add(const std::string &key, bool value) {
        this->obj[key] = value;
        return *this;
    }

    JsonBuilder &add(const std::string &key, int value) {
        this->obj[key] = value;
        return *this;
    }

    JsonBuilder &add(const std::string &key, unsigned long long value) {
        this->obj[key] = value;
        return *this;
    }

    JsonBuilder &add(const std::string &key, float value) {
        this->obj[key] = value;
        return *this;
    }

    JsonBuilder &add(const std::string &key, double value) {
        this->obj[key] = value;
        return *this;
    }

    JsonBuilder &add(const std::string &key, const std::string &value) {
        this->obj[key] = value;
        return *this;
    }

    JsonBuilder &add(const std::string &key, const Json::Value &value) {
        this->obj[key] = value;
        return *this;
    }

private:

    Json::Value obj;
};


#endif //JSON_JSONBUILDER_H
