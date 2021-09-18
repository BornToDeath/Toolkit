
#include "Request/Requests/GetFileRequest.h"

GetFileRequest::GetFileRequest(std::map<std::string, std::string> *params, std::string filePath)
        : GetRequest(params), filePath(std::move(filePath)), file(nullptr) {
    this->type = REQUEST_TYPE_GET_FILE;
    this->needResume = true;
}

