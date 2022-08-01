//
// Created by lixiaoqing on 2022/8/1.
//

#include "ServerListener.h"
#include "Log.h"

#define TAG "ServerListener"

EnHandleResult ServerListener::OnPrepareListen(ITcpServer *pSender, SOCKET soListen) {
    return HR_IGNORE;
}

EnHandleResult ServerListener::OnAccept(ITcpServer *pSender, CONNID dwConnID, UINT_PTR soClient) {
    Log::info(TAG, "OnAccept|ID=%lu", dwConnID);
    return HR_OK;
}

EnHandleResult ServerListener::OnHandShake(ITcpServer *pSender, CONNID dwConnID) {
    return HR_IGNORE;
}

EnHandleResult ServerListener::OnReceive(ITcpServer *pSender, CONNID dwConnID, int iLength) {
    return HR_IGNORE;
}

EnHandleResult ServerListener::OnReceive(ITcpServer *pSender, CONNID dwConnID, const BYTE *pData, int iLength) {
    Log::info(TAG, "OnReceive|ID=%lu, size=%d, msg=[%s]", dwConnID, iLength, pData);
    return HR_OK;
}

EnHandleResult ServerListener::OnSend(ITcpServer *pSender, CONNID dwConnID, const BYTE *pData, int iLength) {
    Log::info(TAG, "OnSend|ID=%lu, size=%d", dwConnID, iLength);
    return HR_OK;
}

EnHandleResult
ServerListener::OnClose(ITcpServer *pSender, CONNID dwConnID, EnSocketOperation enOperation, int iErrorCode) {
    Log::info(TAG, "OnClose|ID=%lu, op=%d, errno=%d(%s)", dwConnID, enOperation, iErrorCode, SYS_GetLastErrorStr());
    return HR_OK;
}

EnHandleResult ServerListener::OnShutdown(ITcpServer *pSender) {
    Log::info(TAG, "OnShutdown");
    return HR_IGNORE;
}