//
// Created by lixiaoqing on 2022/8/1.
//

#include <mutex>
#include "ServerService.h"

static IServerService *singleton = nullptr;
static std::mutex singletonMutex;

IServerService *IServerService::getSingleton() {
    if (singleton == nullptr) {
        static std::once_flag flag;
        std::call_once(flag, []() {
            singleton = new ServerService();
        });
    }
    return singleton;
}

ServerService::ServerService() {

}

ServerService::~ServerService() noexcept {
    stop();
    listenerPtr = nullptr;
    serverPtr = nullptr;
}

void IServerService::releaseSingleton() {
    if (singleton) {
        delete singleton;
        singleton = nullptr;
    }
}

bool ServerService::start(const char *ip, int port) {

    listenerPtr = std::make_shared<ServerListener>();
    serverPtr = std::make_shared<CTcpPackServerPtr>(listenerPtr.get());
    auto server = serverPtr->Get();
    server->SetMaxPackSize(0x1FFFFF);
    server->SetPackHeaderFlag(0x169);
    // TODO 在 Linux 环境下，以下两行开启后程序 crash, why?
//    server->SetKeepAliveTime(5 * 1000L);
//    server->SetKeepAliveInterval(5 * 1000L);
    server->SetSocketBufferSize(1024 * 1024);

    if (serverPtr == nullptr) {
        return false;
    }
    if (isStart.load()) {
        return true;
    }
    bool isOk = serverPtr->Get()->Start(ip, port);
    isStart.store(isOk);
    return isOk;
}

bool ServerService::stop() {
    if (serverPtr == nullptr || !isStart.load()) {
        return true;
    }
    bool isOk = serverPtr->Get()->Stop();
    if (isOk) {
        isStart.store(false);
    }
    return isOk;

}