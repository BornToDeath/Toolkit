//
// Created by lixiaoqing on 2021/9/18.
//

#ifndef NETWORK_REQUEST_H
#define NETWORK_REQUEST_H

#include <string>
#include <vector>
#include "RequestType.h"


class Request {

protected:

    Request();

public:

    virtual ~Request();

public:

    /**
     * 增加新的 Header 键值对
     */
    void appendHeader(const std::string& header);

    /**
     * 返回 Headers
     */
    std::vector<std::string> *getHeaders();

    void setTimeout(int connectTimeoutS, int timeOutS);

public:

    friend class RequestCreator;

    RequestType type;

    std::string url;

    /**
     * 与服务器建立连接前的超时时间，单位：秒
     */
    int connectTimeoutS;

    /**
     * 与服务器建立连接后的超时时间，单位：秒
     */
    int timeOutS;

    bool isPut;

private:

    std::vector<std::string> *mHeaders;
};


#endif //NETWORK_REQUEST_H
