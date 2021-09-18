//
// Created by lixiaoqing on 2021/9/18.
//

#ifndef NETWORK_INETWORKCLIENT_H
#define NETWORK_INETWORKCLIENT_H

#include <functional>
#include "ResultCode.h"
#include "Request/Request.h"

typedef std::function<void(Request *, ResultCode)> NetworkCallback;


class INetworkClient {

public:

    virtual ~INetworkClient() = default;

    /**
     * 单例
     * @return
     */
    static INetworkClient *getSingleton();

    /**
     * 网络请求
     * @param request
     * @param isDebug
     * @return
     */
    virtual ResultCode request(Request *request, bool isDebug = false) = 0;

    /**
     * 异步网络请求
     *
     * @param callback [IN] 回调是在网络请求调用的
     */
    virtual void requestAsyn(Request *request, NetworkCallback callback, bool isDebug = false) = 0;

};


#endif //NETWORK_INETWORKCLIENT_H
