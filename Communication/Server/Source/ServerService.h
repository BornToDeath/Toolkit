//
// Created by lixiaoqing on 2022/8/1.
//

#ifndef TOOLKIT_SERVERSERVICE_H
#define TOOLKIT_SERVERSERVICE_H

#include <memory>
#include <atomic>
#include "IServerService.h"
#include "ServerListener.h"

class ServerService final : public IServerService {

public:

    ServerService();

    ~ServerService() override;

    bool start(const char *ip, int port) override;

    bool stop() override;

private:

    std::shared_ptr<ServerListener> listenerPtr;

    std::shared_ptr<CTcpPackServerPtr> serverPtr;

    std::atomic<bool> isStart{false};
};


#endif //TOOLKIT_SERVERSERVICE_H
