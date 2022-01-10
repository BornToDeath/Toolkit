//
// Created by lixiaoqing on 2021/5/27.
//

#ifndef AIDOT_CLIENTSERVICE_H
#define AIDOT_CLIENTSERVICE_H


#include <memory>
#include "IClientService.h"
#include "ClientListener.h"
#include "Thread/HandlerThread.h"
#include "hpsocket/SocketInterface.h"
#include "hpsocket/HPTypeDef.h"


/**
 * Socket Client 客户端
 */
class ClientService final : public IClientService {

public:

    ClientService();

    ~ClientService() override;

    ClientService(const ClientService &client) = delete;

    ClientService &operator=(const ClientService *client) = delete;

private:

    /**
     * 连接服务端
     * @param address
     * @param port
     * @return
     */
    bool connect(const char *address, int port) override;

    /**
     * 连接固定 [IP, Port] 时调用此方法
     * @return
     */
    bool connect() override;

    /**
     * 断开与服务端的连接并释放所有资源
     * @return
     */
    bool disconnect() override;

    /**
     * 向服务端发送数据
     * @param data
     * @param length
     * @param onSucceed
     * @param onFailed
     */
    void send(const BYTE *data, int length, onAfterSendCallback onSucceed, onAfterSendCallback onFailed) override;

    /**
     * 向服务端发送数据
     * @param dataType
     * @param data
     * @param length
     * @param onSucceed
     * @param onFailed
     */
    void send(int dataType, const BYTE *data, int length, onAfterSendCallback onSucceed,
              onAfterSendCallback onFailed) override;

    /**
     * 接收服务端发来的数据
     */
    void receive() override {}

    /**
     * 是否建立 Socket 连接
     * @return
     */
    bool isConnected() override;

private:

    /**
     * Socket 连接之前的初始化工作
     */
    void init();

private:

    /**
     * Socket 监听器
     */
    std::shared_ptr<ClientListener> listener;

    /**
     * Socket Client 对象智能指针
     */
    std::shared_ptr<CTcpPackClientPtr> clientPtr;

    /**
     * 向服务端发送数据的线程
     */
    std::shared_ptr<HandlerThread> sendThread;

    /**
     * 用于创建 sendThread 的互斥锁
     */
    std::mutex sendThreadMutex;

    /**
     * 是否循环连接的标志
     */
    std::atomic<bool> loopConnectFlag;

};


#endif //AIDOT_CLIENTSERVICE_H