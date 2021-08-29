//
// Created by lixiaoqing on 2021/8/29.
//

#ifndef THREAD_WAITHELPER_H
#define THREAD_WAITHELPER_H

#include <functional>
#include <mutex>
#include <condition_variable>


/**
 * 封装的 wait 相关的工具。
 */
class WaitHelper {

public:

    WaitHelper();

    ~WaitHelper() = default;

    WaitHelper(const WaitHelper &) = delete;

    WaitHelper &operator=(const WaitHelper &) = delete;

public:

    /**
     * 阻塞当前线程，直到满足条件。
     */
    void waitUntil(std::function<bool()> prediction);

    /**
     * 等待，直到满足等待时长或者条件。
     */
    bool waitFor(unsigned long long mills, std::function<bool()> prediction);

    /**
     * 判断是否正在等待中
     */
    bool isWaiting() const;

    /**
     * 通知等待的线程。
     */
    void notify();

    /**
     * 上锁。注意这里是不可重入锁！
     */
    void lock();

    /**
     * 解锁
     */
    void unlock();

private:

    /**
     * 标记是否正在等待。
     */
    bool isWaiting_;

    /**
     * 用于 condition 的锁
     */
    std::mutex mutex;

    /**
     * 条件变量
     */
    std::condition_variable condition;

};


#endif //THREAD_WAITHELPER_H
