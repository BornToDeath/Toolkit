//
// Created by lixiaoqing on 2021/7/12.
//

#ifndef BUSINESSLAYER_CONNECTIONHELPER_H
#define BUSINESSLAYER_CONNECTIONHELPER_H


/**
 * Socket 连接时的助手
 */
class ConnectionHelper final {

public:

    ConnectionHelper() = delete;

    ~ConnectionHelper() = delete;

    ConnectionHelper(const ConnectionHelper &obj) = delete;

    ConnectionHelper &operator=(const ConnectionHelper &obj) = delete;

public:

    /**
     * 建立 adb forward
     * @return
     */
    static bool adbForward();

};


#endif //BUSINESSLAYER_CONNECTIONHELPER_H
