#include "request/requests/get_request.h"

GetRequest::GetRequest(std::map<std::string, std::string> *params) {
    this->params_ = params;
}

