//
// Created by lixiaoqing on 2022/8/1.
//

#ifndef TOOLKIT_ISERVERSERVICE_H
#define TOOLKIT_ISERVERSERVICE_H

/**
 * 服务端
 */
class IServerService {

public:

    static IServerService *getSingleton();

    static void releaseSingleton();

    virtual ~IServerService() {};

    virtual bool start(const char *ip, int port) = 0;

    virtual bool stop() = 0;

};

#endif //TOOLKIT_ISERVERSERVICE_H
