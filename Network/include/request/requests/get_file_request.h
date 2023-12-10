#ifndef NETWORK_GET_FILE_REQUEST_H
#define NETWORK_GET_FILE_REQUEST_H

#include "get_request.h"


class GetFileRequest : public GetRequest {

public:

    GetFileRequest(std::map<std::string, std::string> *params, std::string filepath);

    ~GetFileRequest() override = default;

public:

    std::string filepath_;

    /**
     * 是否需要断点续传
     */
    bool need_resume_;

    FILE *file_;
};

#endif
