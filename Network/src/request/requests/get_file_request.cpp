#include "request/requests/get_file_request.h"

GetFileRequest::GetFileRequest(std::map<std::string, std::string> *params, std::string filepath)
        : GetRequest(params), filepath_(std::move(filepath)), file_(nullptr) {
    this->type_ = REQUEST_TYPE_GET_FILE;
    this->need_resume_ = true;
}

