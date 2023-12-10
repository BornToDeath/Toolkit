#ifndef NETWORK_GET_STRING_REQUEST_H
#define NETWORK_GET_STRING_REQUEST_H


#include "get_request.h"


class GetStringRequest : public GetRequest {

public:

    GetStringRequest(std::map<std::string, std::string> *params, std::string &response);

    ~GetStringRequest() override = default;

public:

    std::string &response_;

};


#endif
