//
// Created by lixiaoqing on 2021/5/27.
//

#ifndef COMMUNICATION_CLIENT_SERVICE_H
#define COMMUNICATION_CLIENT_SERVICE_H


#include <functional>
#include <memory>

using BYTE = unsigned char;


// 发送数据之后的回调
using OnAfterSendCallback = std::function<void(void)>;


/**
 * Socket Client 接口类，定义 Client 的一些功能
 */
class ClientService {

public:

    virtual ~ClientService() = default;

    /**
     * 单例
     * @return
     */
    static ClientService *GetSingleton();

    /**
     * 单例释放
     */
    static void ReleaseSingleton();

    /**
     * 连接服务端
     * @param address 服务端 IP
     * @param port    服务端 Port
     * @return
     */
    virtual bool Connect(const char *address, int port) = 0;

    /**
     * 断开与服务端的 Socket 连接并释放所有资源
     * @return
     */
    virtual bool Disconnect() = 0;

    /**
     * 发送数据。发送之后会自动释放 data
     * @param data      待发送的数据
     * @param length    待发送数据的长度
     * @param on_succeed 发送成功之后的处理
     * @param on_failed  发送失败之后的处理
     * @return
     */
    virtual void Send(const BYTE *data, int length, OnAfterSendCallback on_succeed = nullptr,
                      OnAfterSendCallback on_failed = nullptr) = 0;

    /**
     * 向服务端发送数据
     * @param data_type
     * @param data
     * @param length
     * @param on_succeed 发送成功之后的处理
     * @param on_failed  发送失败之后的处理
     * @return
     */
    virtual void Send(int data_type, const BYTE *data, int length, OnAfterSendCallback on_succeed = nullptr,
                      OnAfterSendCallback on_failed = nullptr) = 0;

    /**
     * 接收服务端发来的数据
     */
    virtual void Receive() = 0;

    /**
     * Socket 连接是否已建立
     * @return
     */
    virtual bool IsConnected() = 0;

};


#endif //COMMUNICATION_CLIENT_SERVICE_H
