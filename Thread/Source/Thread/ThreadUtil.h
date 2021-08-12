//
// Created by lixiaoqing on 2021/8/12.
//

#ifndef THREAD_THREADUTIL_H
#define THREAD_THREADUTIL_H


#include <chrono>

using Timestamp = unsigned long long;


class ThreadUtil {

public:

    static Timestamp timeMills() {
        auto duration = std::chrono::steady_clock::now().time_since_epoch();
        auto mills = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
        return mills;
    }

};


#endif //THREAD_THREADUTIL_H
