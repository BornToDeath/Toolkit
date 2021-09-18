#ifndef SMARTEYE_MODULE_NETWORK_CLIENT_H
#define SMARTEYE_MODULE_NETWORK_CLIENT_H

#include <iostream>
#include "INetworkClient.h"
#include "Request/Request.h"
#include "ResultCode.h"
#include "ThreadPool/ThreadPool.h"


class NetworkClient : public INetworkClient {

public:

    NetworkClient();

    ~NetworkClient() override;

    ResultCode request(Request * request, bool isDebug = false) override;

    /**
     * 异步网络请求
     *
     * @param callback [IN] 回调是在网络请求调用的
     */
    void requestAsyn(Request * request, NetworkCallback callback, bool isDebug = false) override;

private:

    ThreadPool *mThreadPool;
};


#endif
