//
// Created by lixiaoqing on 2021/5/27.
//

/********** 头文件 **********/
#include <cstring>
#include "ClientListener.h"
#include "Log.h"
#include "NotificationCenter.h"
#include "Common/CommonUtil.h"

/********** 宏定义 **********/
#define TAG "ClientListener"


ClientListener::ClientListener() : dataHandleThread(nullptr) {
}

ClientListener::~ClientListener() {
    Log::info(TAG, "【析构】%s", __PRETTY_FUNCTION__);
    if (dataHandleThread) {
        dataHandleThread->quitSafely();
        dataHandleThread = nullptr;
    }
}


EnHandleResult ClientListener::OnPrepareConnect(ITcpClient *pSender, CONNID dwConnID, SOCKET socket) {
    return HR_OK;
}

EnHandleResult ClientListener::OnConnect(ITcpClient *pSender, CONNID dwConnID) {
    Log::debug(TAG, "OnConnect|ID=%lu", dwConnID);
    return HR_OK;
}

EnHandleResult ClientListener::OnHandShake(ITcpClient *pSender, CONNID dwConnID) {
    return HR_OK;
}

EnHandleResult ClientListener::OnReceive(ITcpClient *pSender, CONNID dwConnID, const BYTE *pData, int iLength) {

    /**
     * 此处:
     * 1. 将接收到的 socket 数据存至队列中，并另启线程处理数据，避免 socket 线程被阻塞
     * 2. 由于是在它处处理接收到的数据，所以需要先将数据备份，否则数据会被 socket 线程释放
     */

    // 先备份一份数据
    auto data = new BYTE[iLength]();
    ::memcpy(data, pData, iLength);

//    int type = CommonUtil::bytes2Int(data, 0);
//    Log::info(TAG, "【接收】数据类型: %d, 数据长度: %d", type, iLength);

    if (!dataHandleThread) {
        std::unique_lock<std::mutex> lock(dataHandleThreadMutex);
        if (!dataHandleThread) {
            dataHandleThread = std::make_shared<HandlerThread>("ReceiveThread");
            // 启动 HandlerThread
            dataHandleThread->start();
        }
    }

    // 在 HandlerThread 中处理数据
    auto runnable = [data, iLength]() {
        auto socketData = std::make_tuple(data, iLength);
//        ISocketCache::getSingleton()->pushData(socketData);
        NotificationCenter::defaultCenter->postNotification("onReceiveSocketData", &socketData);
    };

    if (dataHandleThread) {
        auto handler = dataHandleThread->getHandler();
        if (handler) {
            handler->post(runnable);
        }
    }

    return HR_OK;
}

EnHandleResult ClientListener::OnReceive(ITcpClient *pSender, CONNID dwConnID, int iLength) {
    return HR_OK;
}

EnHandleResult ClientListener::OnSend(ITcpClient *pSender, CONNID dwConnID, const BYTE *pData, int iLength) {
//    Log::debug(TAG, "发送 Socket 数据成功, dwConnID=%lu, iLength=%d", dwConnID, iLength);
    return HR_OK;
}

EnHandleResult
ClientListener::OnClose(ITcpClient *pSender, CONNID dwConnID, EnSocketOperation enOperation, int iErrorCode) {
    Log::warn(TAG, "OnClose|Socket 连接关闭! 导致关闭的操作: %d, errno=%d(%s)", enOperation, errno, SYS_GetLastErrorStr());
    return HR_OK;
}