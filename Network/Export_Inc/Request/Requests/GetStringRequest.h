#ifndef SMARTEYE_MODULE_GET_STRING_REQUEST_H
#define SMARTEYE_MODULE_GET_STRING_REQUEST_H


#include "GetRequest.h"


class GetStringRequest : public GetRequest {

public:

    GetStringRequest(std::map<std::string, std::string> *params, std::string &response);

    ~GetStringRequest() override = default;

public:

    std::string &response;

};


#endif
