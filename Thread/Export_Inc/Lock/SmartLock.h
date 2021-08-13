//
// Created by lixiaoqing on 2021/8/12.
//

#ifndef THREAD_SMARTLOCK_H
#define THREAD_SMARTLOCK_H


#include <iostream>
#include <mutex>


/**
 * 创建时加锁，析构时解锁，类似 std::unique_lock
 */
class SmartLock {

public:

    SmartLock();

    SmartLock(std::mutex &mutex);

    explicit SmartLock(std::mutex &mutex, const std::string name);

    ~SmartLock();

private:

    std::mutex *mMutex;

    std::string name;

};


#endif //THREAD_SMARTLOCK_H
