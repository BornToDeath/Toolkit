//
// Created by lixiaoqing on 2021/8/13.
//

#ifndef THREAD_HANDLERTHREAD_H
#define THREAD_HANDLERTHREAD_H


#include "Thread.h"
#include "Looper/Looper.h"
#include "Handler/Handler.h"


/**
 * 类似 Android 的 HandlerThread 。
 * 内部会自动删除，无需手动调用 delete 。
 */
class HandlerThread : public Thread {

public:

    static HandlerThread *newThread(const std::string &name);

    explicit HandlerThread(const std::string &name);

    ~HandlerThread() override;

    /**
     * 获取当前线程 Handler 对象。内部调用了 getLooper() 。
     * 如果线程已经退出了，返回 nullptr ；如果线程还没启动，会等待直到 Handler 创建完成再返回
     * @return
     */
    Handler *getHandler();

    void setOnFinishCallback(const Runnable &runnable) override;

    void quit();

    void quitSafely();

    /**
     * 退出 Loop 循环，并且在完全退出后，自动删除自己
     * @param isSafely 是否安全地退出
     *                 true : 把所有在当前时刻之前应该执行的消息执行完再退出
     *                 false: 直接移除所有未执行的消息再退出
     */
    void quitThenDeleteSelf(bool isSafely);

protected:

    /**
     * Looper 创建好之后的回调
     */
    void onLooperPrepared();

private:

    /**
     * 获取当前线程 Looper 对象。
     * 如果线程已经退出了，返回 nullptr ；如果线程还没启动，会等待直到 Looper 创建完成再返回
     * @return
     */
    Looper *getLooper();

    /**
     * Looper 停止之后的动作
     */
    void onStopped();

private:

    Looper *looper;

    Handler *handler;

    bool isStopped;

    bool isAutoDelete;

    std::mutex mutex;

    std::condition_variable condition;

};


#endif //THREAD_HANDLERTHREAD_H
