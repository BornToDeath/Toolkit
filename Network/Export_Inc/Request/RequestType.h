//
// Created by lixiaoqing on 2021/9/18.
//

#ifndef NETWORK_REQUESTTYPE_H
#define NETWORK_REQUESTTYPE_H


enum RequestType {

    /**
     * 未指定
     */
    REQUEST_TYPE_UNSPECIFIED = 0,

    /**
     * GET 请求, 返回值是一个字符串
     */
    REQUEST_TYPE_GET_STRING  = 1,

    /**
     * GET 请求，返回值是一个文件，或者需要写入文件
     */
    REQUEST_TYPE_GET_FILE    = 2,

    /**
     * POST 请求，返回值是一个字符串
     */
    REQUEST_TYPE_POST        = 3

};


#endif //NETWORK_REQUESTTYPE_H
