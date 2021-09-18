#ifndef SMARTEYE_MODULE_GET_FILE_REQUEST_H
#define SMARTEYE_MODULE_GET_FILE_REQUEST_H

#include "GetRequest.h"


class GetFileRequest : public GetRequest {

public:

    GetFileRequest(std::map<std::string, std::string> *params, std::string filePath);

    ~GetFileRequest() override = default;

public:

    std::string filePath;

    /**
     * 是否需要断点续传
     */
    bool needResume;

    FILE *file;
};

#endif
