//
// Created by lixiaoqing on 2021/5/27.
//

#include "client_service_impl.h"

/******** 引用头文件 ********/
#include <cassert>
#include <memory>
#include <mutex>

#include <sys/prctl.h>

#include "connection_helper.h"
#include "file/file_util.h"
#include "log.h"

/******** 宏定义 ********/
#define TAG "ClientService"


// 单例对象
static ClientService *client_singleton = nullptr;

// 互斥锁
static std::mutex singleton_mutex;


/**
 * 单例实现
 * @return
 */
ClientService *ClientService::GetSingleton() {
    if (client_singleton == nullptr) {
        std::unique_lock<std::mutex> lock(singleton_mutex);
        if (client_singleton == nullptr) {
            client_singleton = new ClientServiceImpl();
        }
    }
    return client_singleton;
}

void ClientService::ReleaseSingleton() {
    Log::Info(TAG, "释放 ClientService");
    if (client_singleton) {
        std::unique_lock<std::mutex> lock(singleton_mutex);
        if (client_singleton) {
            delete client_singleton;
            client_singleton = nullptr;
        }
    }
}

ClientServiceImpl::ClientServiceImpl() : send_thread_(nullptr), loop_connect_flag_(true) {
    // 初始化
    this->Init();
    if (!client_ptr_) {
        Log::Error(TAG, "Socket 初始化失败, 无法建立连接！");
        assert(client_ptr_ != nullptr);
    }
}

ClientServiceImpl::~ClientServiceImpl() {
    Log::Info(TAG, "【析构】%s", __PRETTY_FUNCTION__);

    // 断开连接
    this->Disconnect();

    if (send_thread_) {
        {
            std::unique_lock<std::mutex> lock(send_thread_mutex_);
            if (send_thread_) {
                send_thread_->QuitSafely();
//                send_thread_->quitThenDeleteSelf(true);  // 智能指针不能使用此方法
                send_thread_ = nullptr;
            }
        }
    }
}

void ClientServiceImpl::Init() {
    // 初始化 Socket 监听器
    listener_ = std::make_shared<ClientListener>();

    // 初始化 Socket 对象智能指针
    client_ptr_ = std::make_shared<CTcpPackClientPtr>(listener_.get());

    // 拿到实际的 Socket 对象
    auto client = client_ptr_->Get();

    // 设置 Socket 通信缓冲区大小
    const unsigned int socket_buf_size = 1 * 1024 * 1024;  // 1MB
    client->SetSocketBufferSize(socket_buf_size);

    /**
     * 下面设置超时时间：
     * 如果连接在 kKeepAliveTime 时间内没有任何数据往来，则进行探测。
     * 探测发包的间隔为 kKeepAliveInterval 。尝试探测的最多次数 HPSocket 默认为 5 次。
     * 所以：超时时间 = kKeepAliveTime + (kKeepAliveInterval * 5)
     */

    // 设置正常心跳包间隔。单位：毫秒。
    const unsigned int kKeepAliveTime = 5 * 1000;
    client->SetKeepAliveTime(kKeepAliveTime);

    // 异常心跳包间隔。单位：毫秒
    const unsigned int kKeepAliveInterval = 1 * 1000;
    client->SetKeepAliveInterval(kKeepAliveInterval);

    // 设置发送文件的最大长度
    client->SetMaxPackSize(0x3FFFFF);

    // 设置包头标识（有效包头标识取值范围 0 ~ 1023/0x3FF，当包头标识为 0 时不校验包头，默认：0）
    client->SetPackHeaderFlag(0x169);
}

bool ClientServiceImpl::Connect(const char *const address, int port) {

    /**
     * 建立连接，如果连接失败，则会重试
     */

    // 重连机制
    auto runnable = [this, address, port]() -> bool {

//        // 设置线程名
//        ::prctl(PR_SET_NAME, Constants::CONNECT_THREAD_NAME.c_str());
//        Log::Info(TAG, "启动 Socket 连接线程!");

        assert(client_ptr_ != nullptr);

        // 循环建立连接，直至成功
        while (this->loop_connect_flag_) {
            auto client = client_ptr_->Get();

            // 先检查是否已连接
            bool is_ok = client->IsConnected();
            if (is_ok) {
                Log::Info(TAG, "Socket 连接已建立, 无需重复建立");
//                std::this_thread::sleep_for(std::chrono::seconds(Constants::SLEEP_SECONDS_30));
                std::this_thread::sleep_for(std::chrono::seconds(1));
                break;
            }

            // 建立连接
            Log::Info(TAG, "Socket 连接建立中...");
            BOOL ret = client->Start(address, port, FALSE);
            if (ret == FALSE) {
                Log::Error(TAG, "Socket 连接失败! errno=%d(%s)", errno, SYS_GetLastErrorStr());
                std::this_thread::sleep_for(std::chrono::seconds(5));
                continue;
            }

            // 此处循环检测是否连接的目的是：避免假连接
            int wait_time = 1000;
            do {
                is_ok = client->IsConnected();
                if (is_ok) {
                    break;
                }
                const int step = 100;
                std::this_thread::sleep_for(std::chrono::milliseconds(step));
                wait_time -= step;
            } while (wait_time > 0 && this->loop_connect_flag_);

            if (is_ok) {
                Log::Info(TAG, "Socket 连接成功! Socket ID=%lu", client->GetConnectionID());
                break;
            } else {
                Log::Error(TAG, "Socket 连接失败(假连接)! errno=%d(%s)", errno, SYS_GetLastErrorStr());
                std::this_thread::sleep_for(std::chrono::seconds(5));
            }
        }

        Log::Info(TAG, "退出 socket 连接循环");
        return true;
    };

    // 连接代码不能放在 detach 线程中 TODO: Why?
    runnable();
    return true;
}

bool ClientServiceImpl::Disconnect() {

    // 关闭循环连接
    this->loop_connect_flag_.store(false);

    if (!client_ptr_) {
        Log::Warn(TAG, "Socket 连接为空");
        return true;
    }

    auto client = client_ptr_->Get();
    if (!client->IsConnected()) {
//        Log::Warn(TAG, "Socket 未连接！");
        return true;
    }

    // 关闭 Socket 连接
    BOOL ret = client->Stop();
    if (ret == FALSE) {
        Log::Warn(TAG, "断开 Socket 连接失败！");
        return false;
    }

    Log::Info(TAG, "成功断开 Socket 连接！");
    return true;
}

void
ClientServiceImpl::Send(const BYTE *data, int length, OnAfterSendCallback on_succeed, OnAfterSendCallback on_failed) {

    if (!send_thread_) {
        std::unique_lock<std::mutex> lock(send_thread_mutex_);
        if (!send_thread_) {
            send_thread_ = std::make_shared<HandlerThread>("SendThread");
            // 启动 HandlerThread
            send_thread_->Start();
        }
        lock.unlock();
        Log::Info(TAG, "启动数据发送线程: SendThread");
    }

    // 发送数据
    auto runnable = [=]() {
        if (data == nullptr || length <= 0) {
            Log::Info(TAG, "发送数据时: data == nullptr || length <= 0");
            return;
        }

        auto exec = [=]() -> bool {

            if (!client_ptr_) {
                Log::Warn(TAG, "Socket 连接为空，发送数据失败！");
                return false;
            }

            auto client = client_ptr_->Get();
            if (!client->IsConnected()) {
                Log::Warn(TAG, "Socket 未连接，发送数据失败！");
                return false;
            }

//            Log::Info(TAG, "准备发送数据，数据长度: %dB", length);

            BOOL ret = FALSE;
            int retry_count = 0;

            while (retry_count < 3) {
                ++retry_count;
                // 向服务端发送数据
                ret = client->Send(data, length);
                if (ret == TRUE) {
                    break;
                }
                Log::Info(TAG, "[第%d次]发送数据到服务端失败, errno=%d(%s)", retry_count, errno, SYS_GetLastErrorStr());
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }

            if (ret == TRUE && on_succeed) {
                // 处理发送成功的回调
                on_succeed();
            } else if (ret == FALSE && on_failed) {
                // 处理发送失败的回调
                on_failed();
            }

            return (ret == TRUE);
        };

        /**
         * 如果发送失败，则直接删除此次发送的数据
         */

        bool isOk = exec();
//        if (!isOk) {
//            this->Connect(Constants::SERVER_IP.c_str(), Constants::SERVER_PORT);
//            // 连接成功之后重新发送当前数据
//            this->Send(data, length, callback);
//            return;
//        }

        // 释放资源
        delete[] data;
    };

    if (send_thread_) {
        Handler *handler = nullptr;
        {
            std::unique_lock<std::mutex> lock(send_thread_mutex_);
            if (send_thread_) {
                handler = send_thread_->GetHandler();
            }
        }
        if (handler) {
            handler->Post(runnable);
        }
    }
}

void ClientServiceImpl::Send(int data_type, const BYTE *data, int length, OnAfterSendCallback on_succeed,
                             OnAfterSendCallback on_failed) {
    Log::Info(TAG, "【发送】数据类型: %d, 数据长度: %d Byte", data_type, length);
    this->Send(data, length, on_succeed, on_failed);
}

bool ClientServiceImpl::IsConnected() {
    if (!client_ptr_) {
        return false;
    }
    return client_ptr_->Get()->IsConnected();
}
