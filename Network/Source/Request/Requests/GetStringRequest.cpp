
#include "Request/Requests/GetStringRequest.h"


GetStringRequest::GetStringRequest(std::map<std::string, std::string> *params, std::string &response)
        : GetRequest(params), response(response) {
    this->type = REQUEST_TYPE_GET_STRING;
}

