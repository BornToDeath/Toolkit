//
// Created by lixiaoqing on 2021/8/29.
//

#ifndef THREADDISPATCHER_WORKFLOW_H
#define THREADDISPATCHER_WORKFLOW_H

#include <list>
#include <memory>
#include <map>
#include "Thread/Runnable.h"


enum class WorkflowActionType : int {
    ACTION_TYPE_NORMAL = 0,
    ACTION_TYPE_FORK = 1
};

// 将 <runnable> 执行在 <threadName> 所在线程的委托
using ExeRunnableDelegate = std::function<void(const std::string &threadName, Runnable runnable)>;

using Notifier = std::function<void()>;

using WorkflowTask = std::function<void()>;

using AsyncWorkflowTask = std::function<void(Notifier)>;

// 任务信息: <线程名, 要执行的事件, 事件类型>
using WorkflowTaskInfo = std::tuple<std::string, AsyncWorkflowTask, WorkflowActionType>;


/**
 * 工作流
 */
class Workflow {

public:

    Workflow() = default;

    ~Workflow() = default;

    Workflow(const Workflow &) = delete;

    Workflow &operator=(const Workflow &) = delete;

public:

    /* ======================================================= */
    /* 初级用法                                                 */
    /* ======================================================= */

    /*
     * 如何使用 Workflow：
     *
     * Workflow::create()
     * -> action ("线程名1", [] {
     *     // do something ...
     * })
     * -> action ("线程名2", [] {
     *     // do something ...
     * })
     * -> start();
     *
     */

    /**
     * 创建一个新的工作流
     * @return
     */
    static Workflow *create();

    /**
     *
     * @param delegate 执行 Runnable 的委托
     * @return
     */
    static Workflow *create(ExeRunnableDelegate delegate);

    /**
     * 在指定线程 thread 执行 action
     * @param thread
     * @param action
     * @param tag
     * @return
     */
    Workflow *action(const std::string &thread, WorkflowTask action, const std::string &tag = "Default");

    /**
     * 启动工作流
     */
    void start();

    /**
     * 当前 action 执行完毕，通知可以执行下一个 action 了
     */
    void notify();

    /* ======================================================= */
    /* 高级用法                                                 */
    /* ======================================================= */

    /**
     * 中断。
     * 不再执行当前 Workflow 的后续步骤( finally 还会执行)。
     */
    void interrupt();

    /**
     * 在 指定线程 执行 action。
     */
    Workflow *action(const std::string &thread, const std::function<void(Workflow *)> &action,
                     const std::string &tag = "Default");

    /**
     * 执行一个异步的任务。
     * 调用方可以在异步操作完成之后，调用 notifier 通知可以执行下一个步骤了。
     */
    Workflow *async(const std::string &thread, const AsyncWorkflowTask &action, const std::string &tag = "Default");

    /**
     * 执行一个异步的任务。
     * 调用方可以在异步操作完成之后，调用 notifier 通知可以执行下一个步骤了。
     */
    Workflow *async(const std::string &thread, std::function<void(Workflow *, Notifier)> action,
                    const std::string &tag = "Default");

    /**
     * 所有 Block 都执行完后，会执行这个 Block。
     * 通常用于清理数据等。
     */
    Workflow *finally(const std::string &thread, WorkflowTask action, const std::string &tag = "Default");

    /**
     * 所有 Block 都执行完后，会执行这个 Block。
     * 通常用于清理数据等。
     */
    Workflow *finally(const std::string &thread, const std::function<void(Workflow *)> &action,
                      const std::string &tag = "Default");

    /**
     * 并行计算当前步骤。
     * 通常用于不需要关心结果的场景。
     */
    Workflow *fork(const std::string &thread, WorkflowTask action, const std::string &tag = "Default");

    /**
     * 并行计算当前步骤。
     * 通常用于不需要关心结果的场景。
     */
    Workflow *
    fork(const std::string &thread, const std::function<void(Workflow *)> &action, const std::string &tag = "Default");

    /**
     * 添加数据到缓存
     */
    void set(const std::string &key, std::shared_ptr<void> obj);

    /**
     * 从缓存读取数据
     */
    template<typename T>
    std::shared_ptr<T> get(const std::string &key) {
        return static_cast<T>(dataCache[key]);
    }


private:

    void onFinally();

private:

    /**
     * 执行 runnable 的委托
     */
    ExeRunnableDelegate exeRunnableDelegate;

    /**
     * 待执行的任务列表: <线程名, 要执行的事件, 事件类型>
     */
    std::list<WorkflowTaskInfo> actions;

    /**
     * 最终的任务
     */
    std::tuple<std::string, WorkflowTask> *finallyBlock;

    /**
     * 自带的缓存。
     * 里面保存的数据不会自动删除。
     */
    std::map<std::string, std::shared_ptr<void>> dataCache;
};


#endif //THREADDISPATCHER_WORKFLOW_H
