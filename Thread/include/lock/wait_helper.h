//
// Created by lixiaoqing on 2021/8/29.
//

#ifndef THREAD_WAIT_HELPER_H
#define THREAD_WAIT_HELPER_H

#include <condition_variable>
#include <functional>
#include <mutex>


/**
 * 封装的 wait 相关的工具。
 */
class WaitHelper {

public:

    WaitHelper() = default;

    ~WaitHelper() = default;

    WaitHelper(const WaitHelper &) = delete;

    WaitHelper &operator=(const WaitHelper &) = delete;

public:

    /**
     * 阻塞当前线程，直到满足条件。
     */
    void WaitUntil(std::function<bool()> prediction);

    /**
     * 等待，直到满足等待时长或者条件。
     */
    bool WaitFor(unsigned long long mills, std::function<bool()> prediction);

    /**
     * 通知等待的线程。
     */
    void Notify();

    /**
     * 上锁。注意这里是不可重入锁！
     */
    void Lock();

    /**
     * 解锁
     */
    void Unlock();

    bool TryLock();

private:

    /**
     * 用于 condition_ 的锁
     */
    std::mutex mutex_;

    /**
     * 条件变量
     */
    std::condition_variable condition_;

};


#endif //THREAD_WAIT_HELPER_H
