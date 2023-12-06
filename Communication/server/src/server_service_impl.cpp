//
// Created by lixiaoqing on 2022/8/1.
//

#include "server_service_impl.h"

#include <mutex>

static ServerService *singleton = nullptr;
static std::mutex singleton_mutex;

ServerService *ServerService::GetSingleton() {
    if (singleton == nullptr) {
        static std::once_flag flag;
        std::call_once(flag, []() {
            singleton = new ServerServiceImpl();
        });
    }
    return singleton;
}

void ServerService::ReleaseSingleton() {
    if (singleton) {
        delete singleton;
        singleton = nullptr;
    }
}

ServerServiceImpl::~ServerServiceImpl() noexcept {
    Stop();
    listener_ptr_ = nullptr;
    server_ptr_ = nullptr;
}

bool ServerServiceImpl::Start(const char *ip, int port) {

    listener_ptr_ = std::make_shared<ServerListener>();
    server_ptr_ = std::make_shared<CTcpPackServerPtr>(listener_ptr_.get());
    auto server = server_ptr_->Get();
    server->SetMaxPackSize(0x1FFFFF);
    server->SetPackHeaderFlag(0x169);
    // TODO 在 Linux 环境下，以下两行开启后程序 crash, why?
//    server->SetKeepAliveTime(5 * 1000L);
//    server->SetKeepAliveInterval(5 * 1000L);
    server->SetSocketBufferSize(1024 * 1024);

    if (server_ptr_ == nullptr) {
        return false;
    }
    if (is_start.load()) {
        return true;
    }
    bool is_ok = server_ptr_->Get()->Start(ip, port);
    is_start.store(is_ok);
    return is_ok;
}

bool ServerServiceImpl::Stop() {
    if (server_ptr_ == nullptr || !is_start.load()) {
        return true;
    }
    bool is_ok = server_ptr_->Get()->Stop();
    if (is_ok) {
        is_start.store(false);
    }
    return is_ok;

}