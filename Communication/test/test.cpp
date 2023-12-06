//
// Created by lixiaoqing on 2021/5/20.
//

#include <cstring>
#include <iostream>
#include <thread>

#include <sys/prctl.h>

#include "client_service.h"
#include "data_wrapper.h"
#include "log.h"
#include "server_service.h"

#define TAG "Test"


namespace Test {

void connection() {
    Log::Info(TAG, "---测试socket通信---");

    // 服务端地址
    const std::string server_address = "192.168.55.2";
    const int server_port = 9000;

    auto client = ClientService::GetSingleton();
    client->Connect(server_address.c_str(), server_port);
//        return;

    const std::string json = R""(
{"versionList":[{"name":"businessVersion","version":"100000"},{"name":"modelVersion","version":"200000"}]}
)"";
    int json_len = static_cast<int>(json.length());
    Log::Debug(TAG, "版本号json: %s", json.c_str());

    int length = 4 + 4 + json_len;
    auto data = new unsigned char[length]();

    int offset = 0;
    int data_type = 50;  // 版本号
    memcpy(data + offset, &data_type, 4);

    offset += 4;
    memcpy(data + offset, &json_len, 4);

    offset += 4;
    memcpy(data + offset, json.c_str(), json_len);

    {
        // 测试覆盖率
        BYTE *data1 = nullptr;
        int data1_len;
        DataWrapper::Wrap(data_type, data, length, &data1, data1_len);

        BYTE *data2 = nullptr;
        int dataLen2;
        int dataType2;
        DataWrapper::Parse(data1, data1_len, data_type, &data2, dataLen2);

        delete[] data1;
        delete[] data2;

        client->IsConnected();
        client->Receive();
    }

    const int kMaxCount = 20;
    int index = 0;
    while (index < kMaxCount) {
        auto data_send = new unsigned char[length]();
        memcpy(data_send, data, length);

        ++index;
        Log::Info(TAG, "第 %d 次发送", index);
        client->Send(data_type, data_send, length, nullptr);
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
//            if (index == 200) {
//                break;
//            }
    }

    while (true) {
        Log::Info(TAG, "休眠中...");
        std::this_thread::sleep_for(std::chrono::seconds(5));
        break;
    }

//        std::this_thread::sleep_for(std::chrono::seconds(20));
    Log::Info(TAG, "--- 退出测试 ---");

    delete[] data;
    ClientService::ReleaseSingleton();
}

void test02() {
    int num = 5;
    int length = sizeof(num);
    char *buf = new char[length];
    memcpy(buf, &num, length);

//        DataType type;
    char *output_data = nullptr;
    int output_len = -1;
//    DataWrapper::Parse(reinterpret_cast<const BYTE *>(buf), length, type, &output_data, output_len);
//
//    int outputType = static_cast<std::underlying_type<DataType>::type>(type);
//    Log::Info(TAG, "输出类型=%d", outputType);
}

void ServerTest() {
    auto server = ServerService::GetSingleton();
    if (server == nullptr) {
        Log::Error(TAG, "server is nullptr");
        return;
    }
    Log::Info(TAG, "start server...");
    bool started = server->Start("127.0.0.1", 9001);
    if (started) {
        Log::Info(TAG, "server started at 127.0.0.1:9001");
    } else {
        Log::Error(TAG, "start server failed");
    }
}
}

int main() {

    // 设置线程名
    ::prctl(PR_SET_NAME, "SocketTest");

    // 测试
    Test::ServerTest();

    Log::Info(TAG, "主程序休眠中...");
    std::this_thread::sleep_for(std::chrono::seconds(100));
    Log::Info(TAG, "进程退出...");
    return 0;
}