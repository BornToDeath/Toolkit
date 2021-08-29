//
// Created by lixiaoqing on 2021/8/29.
//

#include "Workflow.h"
#include "Thread/ThreadLog.h"
#include "Threads.h"

#define TAG "Workflow"


Workflow *Workflow::create() {
    return new Workflow();
}

Workflow *Workflow::create(ExeRunnableDelegate delegate) {
    auto wf = new Workflow();
    wf->exeRunnableDelegate = delegate;
    return wf;
}

Workflow *Workflow::action(const std::string &thread, WorkflowTask action, const std::string &tag) {
    // 将一个同步的 action 转为异步的
    AsyncWorkflowTask block = [action, tag](const Notifier &notifier) {
        logger.i(TAG, "即将执行「%s」", tag.c_str());
        action();
        notifier();
        logger.i(TAG, "执行「%s」完成", tag.c_str());
    };

    // 加入到队列
    WorkflowTaskInfo wrapper(thread, block, WorkflowActionType::ACTION_TYPE_NORMAL);
    actions.push_back(wrapper);
    return this;
}

Workflow *
Workflow::action(const std::string &thread, const std::function<void(Workflow *)> &action, const std::string &tag) {
    WorkflowTask wrapper = [this, action] {
        action(this);
    };
    return this->action(thread, wrapper, tag);
}

void Workflow::start() {
    if (actions.empty()) {
        onFinally();
        return;
    }

    // 取链表首部元素
    auto header = actions.front();
    actions.pop_front();

    std::string threadName;
    AsyncWorkflowTask action;
    WorkflowActionType type;
    std::tie(threadName, action, type) = header;

    // 执行首部元素
    if (type == WorkflowActionType::ACTION_TYPE_NORMAL) {
        // 普通任务
        auto runnable = [this, threadName, action]() {
            if (action) {
                Notifier notifier = [this] {
                    this->notify();
                };
                action(notifier);
            } else {
                notify();
            }
        };
        if (exeRunnableDelegate) {
            exeRunnableDelegate(threadName, runnable);
        } else {
            Threads::postOnThread(threadName, runnable, 0, TAG);
        }
    } else {
        // Fork 任务
        auto runnable = [threadName, action] {
            if (action) {
                action([] {});
            }
        };
        if (exeRunnableDelegate) {
            exeRunnableDelegate(threadName, runnable);
        } else {
            Threads::postOnThread(threadName, runnable, 0, TAG);
        }

        // 立即执行下一个
        notify();
    }

}

void Workflow::notify() {
    start();
}

void Workflow::onFinally() {
    if (finallyBlock == nullptr) {
        delete this;
        return;
    }

    std::string threadName;
    WorkflowTask block;
    std::tie(threadName, block) = (*finallyBlock);
    delete finallyBlock;
    finallyBlock = nullptr;

    auto runnable = [this, block] {
        if (block) {
            block();
        }
        delete this;
    };

    if (exeRunnableDelegate) {
        exeRunnableDelegate(threadName, runnable);
    } else {
        Threads::postOnThread(threadName, runnable, 0, "Workflow.onFinally");
    }
}

void Workflow::interrupt() {
    actions.clear();
}

Workflow *Workflow::async(const std::string &thread, const AsyncWorkflowTask &action, const std::string &tag) {
    auto task = [action, tag](Notifier notifier) {
        logger.i(TAG, "即将执行「%s」", tag.c_str());
        action(std::move(notifier));
        logger.i("Workflow", "执行「%s」完成", tag.c_str());
    };
    WorkflowTaskInfo _action(thread, task, WorkflowActionType::ACTION_TYPE_NORMAL);
    actions.push_back(_action);
    return this;
}

Workflow *
Workflow::async(const std::string &thread, std::function<void(Workflow *, Notifier)> action, const std::string &tag) {
    auto task = [this, action, tag](Notifier notifier) {
        logger.i(TAG, "即将执行「%s」", tag.c_str());
        action(this, std::move(notifier));
        logger.i("Workflow", "执行「%s」完成", tag.c_str());
    };
    WorkflowTaskInfo _action(thread, task, WorkflowActionType::ACTION_TYPE_NORMAL);
    actions.push_back(_action);
    return this;
}

Workflow *Workflow::finally(const std::string &thread, WorkflowTask action, const std::string &tag) {
    auto temp = [action, tag]() {
        logger.i(TAG, "即将执行「%s」", tag.c_str());
        action();
        logger.i("Workflow", "执行「%s」完成", tag.c_str());
    };
    delete finallyBlock;
    finallyBlock = new std::tuple<std::string, WorkflowTask>(thread, temp);
    return this;
}

Workflow *
Workflow::finally(const std::string &thread, const std::function<void(Workflow *)> &action, const std::string &tag) {
    auto temp = [this, action, tag]() {
        logger.i(TAG, "即将执行「%s」", tag.c_str());
        action(this);
        logger.i("Workflow", "执行「%s」完成", tag.c_str());
    };
    delete finallyBlock;
    finallyBlock = new std::tuple<std::string, WorkflowTask>(thread, temp);
    return this;
}

Workflow *Workflow::fork(const std::string &thread, WorkflowTask action, const std::string &tag) {
    // 将一个同步的 action 转为 异步的。
    AsyncWorkflowTask block = [action, tag](const Notifier &notifier) {
        logger.i(TAG, "即将执行「%s」", tag.c_str());
        action();
        // 这里不再需要 notify
        logger.i("Workflow", "执行「%s」完成", tag.c_str());
    };

    // 加入到队列
    WorkflowTaskInfo wrapper(thread, block, WorkflowActionType::ACTION_TYPE_FORK);
    actions.push_back(wrapper);
    return this;
}

Workflow *
Workflow::fork(const std::string &thread, const std::function<void(Workflow *)> &action, const std::string &tag) {
    auto wrapper = [this, action]() {
        action(this);
    };
    fork(thread, wrapper, tag);
    return this;
}

void Workflow::set(const std::string &key, std::shared_ptr<void> obj) {
    dataCache[key] = obj;
}
