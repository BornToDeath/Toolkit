#ifndef SMARTEYE_MODULE_GET_REQUEST_H
#define SMARTEYE_MODULE_GET_REQUEST_H

#include <map>
#include "Request/Request.h"


class GetRequest : public Request {

public:

    explicit GetRequest(std::map<std::string, std::string> *params);

    ~GetRequest() override = default;

public:

    /**
     * Get 请求的参数
     */
    std::map<std::string, std::string> *params;

};

#endif
