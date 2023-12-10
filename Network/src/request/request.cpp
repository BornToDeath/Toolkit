//
// Created by lixiaoqing on 2021/9/18.
//
#include "Request/request.h"

Request::Request() : is_put_(false),
                     type_(REQUEST_TYPE_UNSPECIFIED),
                     timeout_sec_(15),
                     connect_timeout_sec_(15),
                     headers_(nullptr) {
}

Request::~Request() {
    if (headers_) {
        delete headers_;
        headers_ = nullptr;
    }
}


void Request::AppendHeader(const std::string &header) {
    if (!headers_) {
        headers_ = new std::vector<std::string>();
    }
    headers_->push_back(header);
}


std::vector<std::string> *Request::GetHeaders() {
    return headers_;
}

void Request::SetTimeout(int connectTimeoutS, int timeOutS) {
    this->connect_timeout_sec_ = connectTimeoutS;
    this->timeout_sec_ = timeOutS;
}