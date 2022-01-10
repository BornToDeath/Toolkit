//
// Created by lixiaoqing on 2021/5/20.
//

#include <iostream>
#include <thread>
#include <sys/prctl.h>
#include <cstring>
#include "Log.h"
#include "IClientService.h"
#include "DataWrapper.h"

#define TAG "Test"


namespace Test {

    void connection() {
        Log::info(TAG, "---测试socket通信---");

        // 服务端地址
        const std::string serverAddress = "192.168.55.2";
        const int serverPort = 9000;

        auto client = IClientService::getSingleton();
        client->connect(serverAddress.c_str(), serverPort);
        return;

        const std::string json = R""(
{"versionList":[{"name":"businessVersion","version":"100000"},{"name":"modelVersion","version":"200000"}]}
)"";
        int jsonLen = static_cast<int>(json.length());
        Log::debug(TAG, "版本号json: %s", json.c_str());

        int length = 4 + 4 + jsonLen;
        auto data = new unsigned char[length]();

        int offset = 0;
        int dataType = 50;  // 版本号
        memcpy(data + offset, &dataType, 4);

        offset += 4;
        memcpy(data + offset, &jsonLen, 4);

        offset += 4;
        memcpy(data + offset, json.c_str(), jsonLen);

        {
            // 测试覆盖率
            BYTE *data1 = nullptr;
            int dataLen1;
            DataWrapper::wrap(dataType, data, length, &data1, dataLen1);

            BYTE *data2 = nullptr;
            int dataLen2;
            int dataType2;
            DataWrapper::parse(data1, dataLen1, dataType, &data2, dataLen2);

            delete[] data1;
            delete[] data2;

            client->isConnected();
            client->receive();
        }

        const int MAX_COUNT = 20;
        int index = 0;
        while (index < MAX_COUNT) {
            auto dataSend = new unsigned char[length]();
            memcpy(dataSend, data, length);

            ++index;
            Log::info(TAG, "第 %d 次发送", index);
            client->send(dataType, dataSend, length, nullptr);
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
//            if (index == 200) {
//                break;
//            }
        }

        while (true) {
            Log::info(TAG, "休眠中...");
            std::this_thread::sleep_for(std::chrono::seconds(5));
            break;
        }

//        std::this_thread::sleep_for(std::chrono::seconds(20));
        Log::info(TAG, "--- 退出测试 ---");

        delete[] data;
        IClientService::releaseSingleton();
    }

    void test02() {
        int num = 5;
        int length = sizeof(num);
        char *buf = new char[length];
        memcpy(buf, &num, length);

//        DataType type;
        char *outputData = nullptr;
        int outputLen = -1;
//    DataWrapper::parse(reinterpret_cast<const BYTE *>(buf), length, type, &outputData, outputLen);
//
//    int outputType = static_cast<std::underlying_type<DataType>::type>(type);
//    Log::info(TAG, "输出类型=%d", outputType);
    }
}

int main() {

    std::string logRootDir = "/mnt/aidot/logs/";
    Log::init(logRootDir.c_str());

    // 设置线程名
    ::prctl(PR_SET_NAME, "SocketTest");

    // 测试
    Test::connection();

    Log::info(TAG, "主程序休眠中...");
    std::this_thread::sleep_for(std::chrono::seconds(100));
    Log::info(TAG, "即将释放 socket 资源");
    IClientService::releaseSingleton();
    Log::info(TAG, "进程退出...");
    return 0;
}