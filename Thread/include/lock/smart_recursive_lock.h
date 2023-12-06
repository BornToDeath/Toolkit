//
// Created by lixiaoqing on 2021/8/13.
//

#ifndef THREAD_SMART_RECURSIVE_LOCK_H
#define THREAD_SMART_RECURSIVE_LOCK_H


#include <iostream>
#include <mutex>


/**
 * 智能递归锁。创建时加锁，析构时解锁
 */
class SmartRecursiveLock {

public:

    explicit SmartRecursiveLock(std::recursive_mutex &mutex);

    explicit SmartRecursiveLock(std::recursive_mutex &mutex, const std::string &name);

    ~SmartRecursiveLock();

private:

    std::recursive_mutex *mutex_;

    std::string name_;

};


#endif //THREAD_SMART_RECURSIVE_LOCK_H
