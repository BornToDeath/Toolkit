#include "request/requests/get_string_request.h"

GetStringRequest::GetStringRequest(std::map<std::string, std::string> *params, std::string &response)
        : GetRequest(params), response_(response) {
    this->type_ = REQUEST_TYPE_GET_STRING;
}

