//
// Created by lixiaoqing on 2022/8/1.
//

#ifndef TOOLKIT_ISERVERSERVICE_H
#define TOOLKIT_ISERVERSERVICE_H

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

#endif //TOOLKIT_ISERVERSERVICE_H
