//
// Created by lixiaoqing on 2022/8/1.
//

#ifndef TOOLKIT_SERVERSERVICE_H
#define TOOLKIT_SERVERSERVICE_H

#include <memory>
#include <atomic>
#include "server_service.h"
#include "server_listener.h"

class ServerServiceImpl final : public ServerService {

public:

    ServerServiceImpl() = default;

    ~ServerServiceImpl() override;

    bool Start(const char *ip, int port) override;

    bool Stop() override;

private:

    std::shared_ptr<ServerListener> listener_ptr_;

    std::shared_ptr<CTcpPackServerPtr> server_ptr_;

    std::atomic<bool> is_start{false};
};


#endif //TOOLKIT_SERVERSERVICE_H
