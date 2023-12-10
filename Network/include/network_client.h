//
// Created by lixiaoqing on 2021/9/18.
//

#ifndef NETWORK_NETWORK_CLIENT_H
#define NETWORK_NETWORK_CLIENT_H

#include <functional>

#include "request/request.h"
#include "result_code.h"

typedef std::function<void(Request *, ResultCode)> NetworkCallback;


class NetworkClient {

public:

    virtual ~NetworkClient() = default;

    /**
     * 单例
     * @return
     */
    static NetworkClient *GetSingleton();

    /**
     * 网络请求
     * @param request
     * @param isDebug
     * @return
     */
    virtual ResultCode Request(Request *request, bool isDebug = false) = 0;

    /**
     * 异步网络请求
     *
     * @param callback [IN] 回调是在网络请求调用的
     */
    virtual void RequestAsync(class Request *request, NetworkCallback callback, bool isDebug = false) = 0;

};


#endif //NETWORK_NETWORK_CLIENT_H
