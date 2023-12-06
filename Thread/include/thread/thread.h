//
// Created by lixiaoqing on 2021/8/12.
//

#ifndef THREAD_THREAD_H
#define THREAD_THREAD_H


#include <string>
#include <thread>

#include "leak_companion.h"
#include "runnable.h"
#include "thread_local.h"


class Thread {

public:

    explicit Thread(const Runnable &runnable);

    virtual ~Thread();

public:

    /**
     * 启动线程
     * 注意：线程在析构时会自动退出
     */
    virtual void Start();

    /**
     * 设置线程结束时的回调。
     * 该回调是在执行完构造函数中传入的 Runnable 后执行。
     */
    virtual void SetOnFinishCallback(const Runnable &runnable);

    /**
     * 设置线程名
     * @param name
     */
    virtual void SetName(const std::string &name);

    /**
     * 获取线程名
     * @return
     */
    virtual std::string GetName();

    /**
     * 当前线程 Sleep 指定毫秒数。
     */
    static void Sleep(unsigned int mills);

    /**
     * 获取当前线程。
     * 如果当前线程不是用 这个模块 创建出来的，返回的可能是一个空。
     */
    static Thread *CurrentThread();

    /**
     * 当前系统的 gmt1970Mills 时间。
     */
    static unsigned long long CurrentTimeMills();

protected:

    /**
     * 仅对子类可见，用这个构造方法创建的线程需要调用 SetRunnable() 设置 Runnable 。
     */
    Thread();

    /**
     * 设置要执行的 Runnable。
     *
     * 在 start() 方法之后调用无效。
     */
    virtual void SetRunnable(const Runnable &runnable);

public:

    /**
     * 线程创建时的回调。
     */
    static std::function<void(const std::string &threadName)> on_thread_start_listener_;

protected:

    /**
     * 线程名
     */
    std::string name_;

    /**
     * 真实的系统的线程对象
     */
    std::thread *thread_{nullptr};

    /**
     * 是否已经启动了。标记 Start() 方法是否被调用过。
     */
    bool is_started_;

    /**
     * 线程主要运行的代码
     */
    Runnable runnable_;

    /**
     * 线程即将退出时的回调
     */
    Runnable on_finish_callback_;

    /**
     * 用于检测内存泄露
     */
    LeakCompanion leak_companion_;

private:

    /**
     * 线程局部存储，存放线程对象
     */
    static ThreadLocal<Thread> *local_thread_;
};


#endif //THREAD_THREAD_H
