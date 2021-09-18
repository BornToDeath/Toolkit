//
// Created by lixiaoqing on 2021/9/18.
//

#include "Request/RequestCreator.h"

/**
 * 返回值为string字符串
 */
GetStringRequest *
RequestCreator::getString(std::string url, std::map<std::string, std::string> *params, std::string &response) {
    //    INFO_LOG("url = %s", url.c_str());
    GetStringRequest *request = new GetStringRequest(params, response);
    request->url = url;
    return request;
}

/**
 * 返回值为一个文件，或者需要把返回值保存到文件的请求
 */
GetFileRequest *RequestCreator::getFile(std::string url, std::map<std::string, std::string> *params, std::string path,
                                        bool needResume) {
    //    INFO_LOG("url = %s", url.c_str());
    GetFileRequest *request = new GetFileRequest(params, std::move(path));
    request->url = std::move(url);
    request->needResume = needResume;
    return request;
}

/**
 * post请求
 */
PostRequest *RequestCreator::post(std::string url, std::vector<PostBody *> *bodies, std::string &response) {
//    Log::info("Http", "请求url = %s ",url.c_str());
    PostRequest *request = new PostRequest(bodies, response);
    request->url = std::move(url);
    return request;
}