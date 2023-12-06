//
// Created by lixiaoqing on 2021/5/27.
//

/********** 头文件 **********/
#include "client_listener.h"

#include <cstring>

#include "common//common_util.h"
#include "log.h"
#include "notification_center.h"

/********** 宏定义 **********/
#define TAG "ClientListener"

ClientListener::~ClientListener() {
    Log::Info(TAG, "【析构】%s", __PRETTY_FUNCTION__);
    is_stopped_.store(true);
    if (data_handle_thread_) {
        std::unique_lock<std::mutex> lock(data_handle_thread_mutex_);
        data_handle_thread_->QuitSafely();
        data_handle_thread_ = nullptr;
    }
}


EnHandleResult ClientListener::OnPrepareConnect(ITcpClient *pSender, CONNID dwConnID, SOCKET socket) {
    return HR_OK;
}

EnHandleResult ClientListener::OnConnect(ITcpClient *pSender, CONNID dwConnID) {
    Log::Debug(TAG, "OnConnect|ID=%lu", dwConnID);
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

    if (is_stopped_.load()) {
        return HR_IGNORE;
    }

    // 先备份一份数据
    auto data = new BYTE[iLength]();
    ::memcpy(data, pData, iLength);

//    int type = CommonUtil::bytes2Int(data, 0);
//    Log::info(TAG, "【接收】数据类型: %d, 数据长度: %d", type, iLength);

    if (!data_handle_thread_) {
        std::unique_lock<std::mutex> lock(data_handle_thread_mutex_);
        if (!data_handle_thread_) {
            data_handle_thread_ = std::make_shared<HandlerThread>("ReceiveThread");
            // 启动 HandlerThread
            data_handle_thread_->Start();
        }
    }

    // 在 HandlerThread 中处理数据
    auto runnable = [data, iLength]() {
        auto socket_data = std::make_tuple(data, iLength);
//        ISocketCache::GetSingleton()->pushData(socket_data);
        NotificationCenter::default_center_->PostNotification("onReceiveSocketData", &socket_data);
    };

    if (data_handle_thread_) {
        Handler *handler = nullptr;
        {
            std::unique_lock<std::mutex> lock(data_handle_thread_mutex_);
            if (data_handle_thread_) {
                handler = data_handle_thread_->GetHandler();
            }
        }
        if (handler) {
            handler->Post(runnable);
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
    Log::Warn(TAG, "OnClose|Socket 连接关闭! 导致关闭的操作: %d, errno=%d(%s)", enOperation, errno,
              SYS_GetLastErrorStr());
    return HR_OK;
}