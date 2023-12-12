//
// Created by lixiaoqing on 2022/8/1.
//

#ifndef COMMUNICATION_SERVER_SERVICE_H
#define COMMUNICATION_SERVER_SERVICE_H

/**
 * 服务端
 */
class ServerService {
public:
    virtual ~ServerService() {};

public:
    static ServerService *GetSingleton();

    static void ReleaseSingleton();

    virtual bool Start(const char *ip, int port) = 0;

    virtual bool Stop() = 0;
};

#endif //COMMUNICATION_SERVER_SERVICE_H
