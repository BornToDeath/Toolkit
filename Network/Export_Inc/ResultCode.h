//
// Created by lixiaoqing on 2021/9/18.
//

#ifndef NETWORK_RESULTCODE_H
#define NETWORK_RESULTCODE_H

#include <iostream>
#include "curl/curl.h"


enum RequestResult {
    SN_RESULT_CODE_SUCCESS = 0,
    SN_RESULT_CODE_FAILED = 1
};


class ResultCode {

public:

    RequestResult requestResult;

    CURLcode curlCode;

};


#endif //NETWORK_RESULTCODE_H
