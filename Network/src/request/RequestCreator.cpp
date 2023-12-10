//
// Created by lixiaoqing on 2021/9/18.
//
#include "request/request_creator.h"

/**
 * 返回值为string字符串
 */
GetStringRequest *
RequestCreator::GetString(std::string url, std::map<std::string, std::string> *params, std::string &response) {
    //    INFO_LOG("url = %s", url.c_str());
    GetStringRequest *request = new GetStringRequest(params, response);
    request->url_ = url;
    return request;
}

/**
 * 返回值为一个文件，或者需要把返回值保存到文件的请求
 */
GetFileRequest *RequestCreator::GetFile(std::string url, std::map<std::string, std::string> *params, std::string path,
                                        bool need_resume) {
    //    INFO_LOG("url = %s", url.c_str());
    GetFileRequest *request = new GetFileRequest(params, std::move(path));
    request->url_ = std::move(url);
    request->need_resume_ = need_resume;
    return request;
}

/**
 * post请求
 */
PostRequest *RequestCreator::Post(std::string url, std::vector<PostBody *> *bodies, std::string &response) {
//    Log::info("Http", "请求url = %s ",url.c_str());
    PostRequest *request = new PostRequest(bodies, response);
    request->url_ = std::move(url);
    return request;
}