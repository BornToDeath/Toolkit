//
// Created by lixiaoqing on 2021/8/13.
//

#ifndef THREAD_HANDLER_THREAD_H
#define THREAD_HANDLER_THREAD_H


#include "thread.h"
#include "message/looper.h"
#include "message/handler.h"


/**
 * 类似 Android 的 HandlerThread 。
 * 内部会自动删除，无需手动调用 delete 。
 */
class HandlerThread : public Thread {

public:

    static HandlerThread *NewInstance(const std::string &name);

    explicit HandlerThread(const std::string &name);

    ~HandlerThread() override;

    /**
     * 获取当前线程 Handler 对象。内部调用了 GetLooper() 。
     * 如果线程已经退出了，返回 nullptr ；如果线程还没启动，会等待直到 Handler 创建完成再返回
     * @return
     */
    Handler *GetHandler();

    void SetOnFinishCallback(const Runnable &runnable) override;

    void Quit();

    void QuitSafely();

    /**
     * 退出 Loop 循环，并且在完全退出后，自动删除自己. 注意: 不能对智能指针调用此方法!
     * @param is_safely 是否安全地退出
     *                 true : 把所有在当前时刻之前应该执行的消息执行完再退出
     *                 false: 直接移除所有未执行的消息再退出
     */
    void QuitThenDeleteSelf(bool is_safely = true);

protected:

    /**
     * Looper 创建好之后的回调
     */
    void OnLooperPrepared();

private:

    /**
     * 获取当前线程 Looper 对象。
     * 如果线程已经退出了，返回 nullptr ；如果线程还没启动，会等待直到 Looper 创建完成再返回
     * @return
     */
    Looper *GetLooper();

    /**
     * Looper 停止之后的动作
     */
    void OnStopped();

private:

    Looper *looper_;

    Handler *handler_;

    bool is_stopped_;

    bool auto_delete_;

    std::mutex mutex_;

    std::condition_variable condition_;

};


#endif //THREAD_HANDLER_THREAD_H
