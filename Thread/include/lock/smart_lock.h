//
// Created by lixiaoqing on 2021/8/12.
//

#ifndef THREAD_SMART_LOCK_H
#define THREAD_SMART_LOCK_H


#include <iostream>
#include <mutex>


/**
 * 创建时加锁，析构时解锁，类似 std::unique_lock
 */
class SmartLock {

public:

    explicit SmartLock(std::mutex &mutex);

    explicit SmartLock(std::mutex &mutex, const std::string &name);

    ~SmartLock();

private:

    std::mutex *mutex_;

    std::string name_;

};


#endif //THREAD_SMART_LOCK_H
