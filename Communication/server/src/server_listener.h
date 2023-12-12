//
// Created by lixiaoqing on 2022/8/1.
//

#ifndef COMMUNICATION_SERVER_LISTENER_H
#define COMMUNICATION_SERVER_LISTENER_H

#include <hpsocket/HPSocket.h>

class ServerListener : public CTcpServerListener {

public:

    ServerListener() = default;

    ~ServerListener() override = default;

public:

    EnHandleResult OnPrepareListen(ITcpServer *pSender, SOCKET soListen) override;

    EnHandleResult OnAccept(ITcpServer *pSender, CONNID dwConnID, UINT_PTR soClient) override;

    EnHandleResult OnHandShake(ITcpServer *pSender, CONNID dwConnID) override;

    EnHandleResult OnReceive(ITcpServer *pSender, CONNID dwConnID, int iLength) override;

    EnHandleResult OnReceive(ITcpServer *pSender, CONNID dwConnID, const BYTE *pData, int iLength) override;

    EnHandleResult OnSend(ITcpServer *pSender, CONNID dwConnID, const BYTE *pData, int iLength) override;

    // 客户端关闭连接后触发
    EnHandleResult
    OnClose(ITcpServer *pSender, CONNID dwConnID, EnSocketOperation enOperation, int iErrorCode) override;

    // 服务端停止后触发
    EnHandleResult OnShutdown(ITcpServer *pSender) override;

};


#endif //COMMUNICATION_SERVER_LISTENER_H
