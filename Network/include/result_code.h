//
// Created by lixiaoqing on 2021/9/18.
//

#ifndef NETWORK_RESULT_CODE_H
#define NETWORK_RESULT_CODE_H

#include <iostream>

#include "curl/curl.h"


enum RequestResult {
    SN_RESULT_CODE_SUCCESS = 0,
    SN_RESULT_CODE_FAILED = 1
};


class ResultCode {

public:

    RequestResult request_result_;

    CURLcode curl_code_;

};


#endif //NETWORK_RESULT_CODE_H
