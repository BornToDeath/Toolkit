//
// Created by lixiaoqing on 2021/5/27.
//

#ifndef COMMUNICATION_CLIENT_SERVICE_IMPL_H
#define COMMUNICATION_CLIENT_SERVICE_IMPL_H


#include <memory>

#include "client_service.h"
#include "client_listener.h"
#include "thread/handler_thread.h"
#include "hpsocket/HPTypeDef.h"
#include "hpsocket/SocketInterface.h"


/**
 * Socket Client 客户端
 */
class ClientServiceImpl final : public ClientService {

public:

    ClientServiceImpl();

    ~ClientServiceImpl() override;

    ClientServiceImpl(const ClientServiceImpl &client) = delete;

    ClientServiceImpl &operator=(const ClientServiceImpl *client) = delete;

private:

    /**
     * 连接服务端
     * @param address
     * @param port
     * @return
     */
    bool Connect(const char *address, int port) override;

    /**
     * 断开与服务端的连接并释放所有资源
     * @return
     */
    bool Disconnect() override;

    /**
     * 向服务端发送数据
     * @param data
     * @param length
     * @param on_succeed
     * @param on_failed
     */
    void Send(const BYTE *data, int length, OnAfterSendCallback on_succeed, OnAfterSendCallback on_failed) override;

    /**
     * 向服务端发送数据
     * @param data_type
     * @param data
     * @param length
     * @param on_succeed
     * @param on_failed
     */
    void Send(int data_type, const BYTE *data, int length, OnAfterSendCallback on_succeed, OnAfterSendCallback on_failed) override;

    /**
     * 接收服务端发来的数据
     */
    void Receive() override {}

    /**
     * 是否建立 Socket 连接
     * @return
     */
    bool IsConnected() override;

private:

    /**
     * Socket 连接之前的初始化工作
     */
    void Init();

private:

    /**
     * Socket 监听器
     */
    std::shared_ptr<ClientListener> listener_;

    /**
     * Socket Client 对象智能指针
     */
    std::shared_ptr<CTcpPackClientPtr> client_ptr_;

    /**
     * 向服务端发送数据的线程
     */
    std::shared_ptr<HandlerThread> send_thread_;

    /**
     * 用于创建 send_thread_ 的互斥锁
     */
    std::mutex send_thread_mutex_;

    /**
     * 是否循环连接的标志
     */
    std::atomic<bool> loop_connect_flag_;

};


#endif //COMMUNICATION_CLIENT_SERVICE_IMPL_H