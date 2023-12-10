//
// Created by lixiaoqing on 2021/9/18.
//

#ifndef NETWORK_REQUEST_H
#define NETWORK_REQUEST_H

#include <string>
#include <vector>

#include "request_type.h"


class Request {

protected:

    Request();

public:

    virtual ~Request();

public:

    /**
     * 增加新的 Header 键值对
     */
    void AppendHeader(const std::string &header);

    /**
     * 返回 Headers
     */
    std::vector<std::string> *GetHeaders();

    void SetTimeout(int connectTimeoutS, int timeOutS);

public:

    friend class RequestCreator;

    RequestType type_;

    std::string url_;

    /**
     * 与服务器建立连接前的超时时间，单位：秒
     */
    int connect_timeout_sec_;

    /**
     * 与服务器建立连接后的超时时间，单位：秒
     */
    int timeout_sec_;

    bool is_put_;

private:

    std::vector<std::string> *headers_;
};


#endif //NETWORK_REQUEST_H
