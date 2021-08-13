//
// Created by lixiaoqing on 2021/8/13.
//

#ifndef THREAD_SMARTRECURSIVELOCK_H
#define THREAD_SMARTRECURSIVELOCK_H


#include <iostream>
#include <mutex>


/**
 * 智能递归锁。创建时加锁，析构时解锁
 */
class SmartRecursiveLock {

public:

    SmartRecursiveLock(std::recursive_mutex &mutex);

    explicit SmartRecursiveLock(std::recursive_mutex &mutex, const std::string &name);

    ~SmartRecursiveLock();

private:

    std::recursive_mutex *mMutex;

    std::string name;

};


#endif //THREAD_SMARTRECURSIVELOCK_H
