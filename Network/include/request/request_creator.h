//
// Created by lixiaoqing on 2021/9/18.
//

#ifndef NETWORK_REQUEST_CREATOR_H
#define NETWORK_REQUEST_CREATOR_H

#include "request/requests/get_string_request.h"
#include "request/requests/get_file_request.h"
#include "request/requests/post_request.h"


class RequestCreator {

public:

    RequestCreator() = default;

    ~RequestCreator() = default;

public:

    /**
     * 构建一个返回值为字符串的请求
     *
     * @param params    [IN] 接口参数
     * @param response  [OUT] 返回的字符串
     */
    static GetStringRequest *
    GetString(std::string url, std::map<std::string, std::string> *params, std::string &response);

    /**
     * 构建一个返回值为文件，或者需要把返回值保存到文件的请求
     * 通常用于下载文件。
     *
     * @param params      [IN] 接口参数
     * @param need_resume [IN] 是否断点续传，默认是
     * @param response    [OUT] 返回的字符串
     */
    static GetFileRequest *
    GetFile(std::string url, std::map<std::string, std::string> *params, std::string path, bool need_resume = true);

    /**
     * 构建一个 Post 请求，返回值是字符串
     *
     * @param bodies     [IN] Body体
     * @param response   [OUT] 返回的字符串
     */
    static PostRequest *Post(std::string url, std::vector<PostBody *> *bodies, std::string &response);

};


#endif //NETWORK_REQUEST_CREATOR_H
