//
// Created by lixiaoqing on 2021/5/27.
//


/******** 引用头文件 ********/
#include <memory>
#include <mutex>
#include <sys/prctl.h>
#include <cassert>
#include "ClientService.h"
#include "Log.h"
#include "File/FileUtil.h"
#include "ConnectionHelper.h"

/******** 宏定义 ********/
#define TAG "ClientService"


// 单例对象
static IClientService *clientSingleton = nullptr;

// 互斥锁
static std::mutex singletonMutex;


/**
 * 单例实现
 * @return
 */
IClientService *IClientService::getSingleton() {
    if (clientSingleton == nullptr) {
        std::unique_lock<std::mutex> lock(singletonMutex);
        if (clientSingleton == nullptr) {
            clientSingleton = new ClientService();
        }
    }
    return clientSingleton;
}

void IClientService::releaseSingleton() {
    Log::info(TAG, "释放 ClientService");
    if (clientSingleton) {
        std::unique_lock<std::mutex> lock(singletonMutex);
        if (clientSingleton) {
            delete clientSingleton;
            clientSingleton = nullptr;
        }
    }
}

ClientService::ClientService() : sendThread(nullptr), loopConnectFlag(true) {
    // 初始化
    this->init();
    if (!clientPtr) {
        Log::error(TAG, "Socket 初始化失败, 无法建立连接！");
        assert(clientPtr != nullptr);
    }
}

ClientService::~ClientService() {
    Log::info(TAG, "【析构】%s", __PRETTY_FUNCTION__);

    // 断开连接
    this->disconnect();

    if (sendThread) {
        {
            std::unique_lock<std::mutex> lock(sendThreadMutex);
            if (sendThread) {
                sendThread->quitSafely();
//                sendThread->quitThenDeleteSelf(true);  // 智能指针不能使用此方法
                sendThread = nullptr;
            }
        }
    }
}

void ClientService::init() {
    // 初始化 Socket 监听器
    listener = std::make_shared<ClientListener>();

    // 初始化 Socket 对象智能指针
    clientPtr = std::make_shared<CTcpPackClientPtr>(listener.get());

    // 拿到实际的 Socket 对象
    auto client = clientPtr->Get();

    // 设置 Socket 通信缓冲区大小
    const unsigned int socketBufSize = 1 * 1024 * 1024;  // 1MB
    client->SetSocketBufferSize(socketBufSize);

    /**
     * 下面设置超时时间：
     * 如果连接在 keepAliveTime 时间内没有任何数据往来，则进行探测。
     * 探测发包的间隔为 keepAliveInterval 。尝试探测的最多次数 HPSocket 默认为 5 次。
     * 所以：超时时间 = keepAliveTime + (keepAliveInterval * 5)
     */

    // 设置正常心跳包间隔。单位：毫秒。
    const unsigned int keepAliveTime = 5 * 1000;
    client->SetKeepAliveTime(keepAliveTime);

    // 异常心跳包间隔。单位：毫秒
    const unsigned int keepAliveInterval = 1 * 1000;
    client->SetKeepAliveInterval(keepAliveInterval);

    // 设置发送文件的最大长度
    client->SetMaxPackSize(0x3FFFFF);

    // 设置包头标识（有效包头标识取值范围 0 ~ 1023/0x3FF，当包头标识为 0 时不校验包头，默认：0）
    client->SetPackHeaderFlag(0x169);
}

bool ClientService::connect(const char *const address, int port) {

    /**
     * 建立连接，如果连接失败，则会重试
     */

    // 重连机制
    auto runnable = [this, address, port]() -> bool {

//        // 设置线程名
//        ::prctl(PR_SET_NAME, Constants::CONNECT_THREAD_NAME.c_str());
//        Log::info(TAG, "启动 Socket 连接线程!");

        assert(clientPtr != nullptr);

        // 循环建立连接，直至成功
        while (this->loopConnectFlag) {
            auto client = clientPtr->Get();

            // 先检查是否已连接
            bool isOk = client->IsConnected();
            if (isOk) {
                Log::info(TAG, "Socket 连接已建立, 无需重复建立");
//                std::this_thread::sleep_for(std::chrono::seconds(Constants::SLEEP_SECONDS_30));
                std::this_thread::sleep_for(std::chrono::seconds(1));
                break;
            }

            // 建立连接
            Log::info(TAG, "Socket 连接建立中...");
            BOOL ret = client->Start(address, port, FALSE);
            if (ret == FALSE) {
                Log::error(TAG, "Socket 连接失败! errno=%d(%s)", errno, SYS_GetLastErrorStr());
                std::this_thread::sleep_for(std::chrono::seconds(5));
                continue;
            }

            // 此处循环检测是否连接的目的是：避免假连接
            int waitTime = 1000;
            do {
                isOk = client->IsConnected();
                if (isOk) {
                    break;
                }
                const int step = 100;
                std::this_thread::sleep_for(std::chrono::milliseconds(step));
                waitTime -= step;
            } while (waitTime > 0 && this->loopConnectFlag);

            if (isOk) {
                Log::info(TAG, "Socket 连接成功! Socket ID=%lu", client->GetConnectionID());
                break;
            } else {
                Log::error(TAG, "Socket 连接失败(假连接)! errno=%d(%s)", errno, SYS_GetLastErrorStr());
                std::this_thread::sleep_for(std::chrono::seconds(5));
            }
        }

        Log::info(TAG, "退出 socket 连接循环");
        return true;
    };

    // 连接代码不能放在 detach 线程中 TODO: Why?
    runnable();
    return true;
}

bool ClientService::connect() {
    const std::string serverAddress = "192.168.55.2";
    const int serverPort = 9000;
    return this->connect(serverAddress.c_str(), serverPort);
}

bool ClientService::disconnect() {

    // 关闭循环连接
    this->loopConnectFlag.store(false);

    if (!clientPtr) {
        Log::warn(TAG, "Socket 连接为空");
        return true;
    }

    auto client = clientPtr->Get();
    if (!client->IsConnected()) {
//        Log::warn(TAG, "Socket 未连接！");
        return true;
    }

    // 关闭 Socket 连接
    BOOL ret = client->Stop();
    if (ret == FALSE) {
        Log::warn(TAG, "断开 Socket 连接失败！");
        return false;
    }

    Log::info(TAG, "成功断开 Socket 连接！");
    return true;
}

void ClientService::send(const BYTE *data, int length, onAfterSendCallback onSucceed, onAfterSendCallback onFailed) {

    if (!sendThread) {
        std::unique_lock<std::mutex> lock(sendThreadMutex);
        if (!sendThread) {
            sendThread = std::make_shared<HandlerThread>("SendThread");
            // 启动 HandlerThread
            sendThread->start();
        }
        lock.unlock();
        Log::info(TAG, "启动数据发送线程: SendThread");
    }

    // 发送数据
    auto runnable = [=]() {
        if (data == nullptr || length <= 0) {
            Log::info(TAG, "发送数据时: data == nullptr || length <= 0");
            return;
        }

        auto exec = [=]() -> bool {

            if (!clientPtr) {
                Log::warn(TAG, "Socket 连接为空，发送数据失败！");
                return false;
            }

            auto client = clientPtr->Get();
            if (!client->IsConnected()) {
                Log::warn(TAG, "Socket 未连接，发送数据失败！");
                return false;
            }

//            Log::info(TAG, "准备发送数据，数据长度: %dB", length);

            BOOL ret = FALSE;
            int retryCount = 0;

            while (retryCount < 3) {
                ++retryCount;
                // 向服务端发送数据
                ret = client->Send(data, length);
                if (ret == TRUE) {
                    break;
                }
                Log::info(TAG, "[第%d次]发送数据到服务端失败, errno=%d(%s)", retryCount, errno, SYS_GetLastErrorStr());
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }

            if (ret == TRUE && onSucceed) {
                // 处理发送成功的回调
                onSucceed();
            } else if (ret == FALSE && onFailed) {
                // 处理发送失败的回调
                onFailed();
            }

            return (ret == TRUE);
        };

        /**
         * 如果发送失败，则直接删除此次发送的数据
         */

        bool isOk = exec();
//        if (!isOk) {
//            this->connect(Constants::SERVER_IP.c_str(), Constants::SERVER_PORT);
//            // 连接成功之后重新发送当前数据
//            this->send(data, length, callback);
//            return;
//        }

        // 释放资源
        delete[] data;
    };

    if (sendThread) {
        Handler* handler = nullptr;
        {
            std::unique_lock<std::mutex> lock(sendThreadMutex);
            if (sendThread) {
                handler = sendThread->getHandler();
            }
        }
        if (handler) {
            handler->post(runnable);
        }
    }
}

void ClientService::send(int dataType, const BYTE *data, int length, onAfterSendCallback onSucceed, onAfterSendCallback onFailed) {
    Log::info(TAG, "【发送】数据类型: %d, 数据长度: %d Byte", dataType, length);
    this->send(data, length, onSucceed, onFailed);
}

bool ClientService::isConnected() {
    if (!clientPtr) {
        return false;
    }
    return clientPtr->Get()->IsConnected();
}
