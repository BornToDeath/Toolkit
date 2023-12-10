#ifndef NETWORK_NETWORK_CLIENT_IMPL_H
#define NETWORK_NETWORK_CLIENT_IMPL_H

#include <iostream>

#include "network_client.h"
#include "request/request.h"
#include "result_code.h"
#include "thread/thread_pool.h"


class NetworkClientImpl : public NetworkClient {

public:

    NetworkClientImpl();

    ~NetworkClientImpl() override;

    ResultCode Request(class Request *request, bool is_debug = false) override;

    /**
     * 异步网络请求
     *
     * @param callback [IN] 回调是在网络请求调用的
     */
    void RequestAsync(class Request *request, NetworkCallback callback, bool is_debug = false) override;

private:

    ThreadPool *thread_pool_;
};


#endif  // NETWORK_NETWORK_CLIENT_IMPL_H
