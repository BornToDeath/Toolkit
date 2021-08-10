//
// Created by lixiaoqing on 2021/5/19.
//

#include "LogData.h"


LogData::LogData(const LogType type,
                 const LogLevel level,
                 const std::string &tag,
                 std::string log) :
        type(type),
        level(level),
        tag(tag),
        log(std::move(log)) {
    tag.data();
    tag.c_str();
}

LogLevel LogData::getLevel() const {
    return this->level;
}

std::string LogData::getTag() const {
    return this->tag;
}

std::string LogData::getLog() const {
    return this->log;
}

LogType LogData::getType() const {
    return this->type;
}

void LogData::setLog(const std::string &log) {
    this->log = log;
}

