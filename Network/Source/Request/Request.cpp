//
// Created by lixiaoqing on 2021/9/18.
//

#include "Request/Request.h"


Request::Request() : isPut(false),
                     type(REQUEST_TYPE_UNSPECIFIED),
                     timeOutS(15),
                     connectTimeoutS(15),
                     mHeaders(nullptr) {
}

Request::~Request() {
    if (mHeaders) {
        delete mHeaders;
        mHeaders = nullptr;
    }
}


void Request::appendHeader(const std::string& header) {
    if (!mHeaders) {
        mHeaders = new std::vector<std::string>();
    }
    mHeaders->push_back(header);
}


std::vector<std::string> *Request::getHeaders() {
    return mHeaders;
}

void Request::setTimeout(int connectTimeoutS, int timeOutS) {
    this->connectTimeoutS = connectTimeoutS;
    this->timeOutS = timeOutS;
}