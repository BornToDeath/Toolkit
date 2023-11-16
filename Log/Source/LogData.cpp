//
// Created by lixiaoqing on 2021/5/19.
//

#include "LogData.h"

namespace log {

LogData::LogData(const LogType type,
                 const LogLevel level,
                 const std::string &tag,
                 std::string log) :
        type_(type),
        level_(level),
        tag_(tag),
        log_(std::move(log)) {
    tag.data();
    tag.c_str();
}

LogLevel LogData::GetLevel() const {
    return this->level_;
}

std::string LogData::GetTag() const {
    return this->tag_;
}

std::string LogData::GetLog() const {
    return this->log_;
}

LogType LogData::GetType() const {
    return this->type_;
}

void LogData::SetLog(const std::string &log) {
    this->log_ = log;
}

}  // namespace log