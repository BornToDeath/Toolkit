//
// Created by lixiaoqing on 2021/8/12.
//

#ifndef THREAD_THREADUTIL_H
#define THREAD_THREADUTIL_H


#include <chrono>

using Timestamp = unsigned long long;


/**
 * Thread 工具类
 */
class ThreadUtil {

public:

    /**
     * 获取当前系统时间戳，单位：毫秒
     * @return
     */
    static Timestamp timeMills() {
        auto duration = std::chrono::steady_clock::now().time_since_epoch();
        auto mills = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
        return mills;
    }

};


#endif //THREAD_THREADUTIL_H
