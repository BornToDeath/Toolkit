//
// Created by lixiaoqing on 2022/8/1.
//

#include "server_listener.h"
#include "log.h"

#define TAG "ServerListener"

EnHandleResult ServerListener::OnPrepareListen(ITcpServer *pSender, SOCKET soListen) {
    return HR_IGNORE;
}

EnHandleResult ServerListener::OnAccept(ITcpServer *pSender, CONNID dwConnID, UINT_PTR soClient) {
    Log::Info(TAG, "OnAccept|ID=%lu", dwConnID);
    return HR_OK;
}

EnHandleResult ServerListener::OnHandShake(ITcpServer *pSender, CONNID dwConnID) {
    return HR_IGNORE;
}

EnHandleResult ServerListener::OnReceive(ITcpServer *pSender, CONNID dwConnID, int iLength) {
    return HR_IGNORE;
}

EnHandleResult ServerListener::OnReceive(ITcpServer *pSender, CONNID dwConnID, const BYTE *pData, int iLength) {
    Log::Info(TAG, "OnReceive|ID=%lu, size=%d, msg=[%s]", dwConnID, iLength, pData);
    return HR_OK;
}

EnHandleResult ServerListener::OnSend(ITcpServer *pSender, CONNID dwConnID, const BYTE *pData, int iLength) {
    Log::Info(TAG, "OnSend|ID=%lu, size=%d", dwConnID, iLength);
    return HR_OK;
}

EnHandleResult
ServerListener::OnClose(ITcpServer *pSender, CONNID dwConnID, EnSocketOperation enOperation, int iErrorCode) {
    Log::Info(TAG, "OnClose|ID=%lu, op=%d, errno=%d(%s)", dwConnID, enOperation, iErrorCode, SYS_GetLastErrorStr());
    return HR_OK;
}

EnHandleResult ServerListener::OnShutdown(ITcpServer *pSender) {
    Log::Info(TAG, "OnShutdown");
    return HR_IGNORE;
}