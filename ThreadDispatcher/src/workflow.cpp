//
// Created by lixiaoqing on 2021/8/29.
//

#include "workflow.h"

#include <utility>

#include "threads.h"
#include "thread/thread_log.h"

#define TAG "Workflow"


Workflow *Workflow::Create() {
    return new Workflow();
}

Workflow *Workflow::Create(ExeRunnableDelegate delegate) {
    auto wf = new Workflow();
    wf->exe_runnable_delegate_ = std::move(delegate);
    return wf;
}

Workflow *Workflow::Action(const std::string &thread, WorkflowTask action, const std::string &tag) {
    // 将一个同步的 Action 转为异步的
    AsyncWorkflowTask block = [action, tag](const Notifier &notifier) {
        logger.i(TAG, "即将执行「%s」", tag.c_str());
        action();
        notifier();
        logger.i(TAG, "执行「%s」完成", tag.c_str());
    };

    // 加入到队列
    WorkflowTaskInfo wrapper(thread, block, WorkflowActionType::ACTION_TYPE_NORMAL);
    actions_.push_back(wrapper);
    return this;
}

Workflow *
Workflow::Action(const std::string &thread, const std::function<void(Workflow *)> &action, const std::string &tag) {
    WorkflowTask wrapper = [this, action] {
        action(this);
    };
    return this->Action(thread, wrapper, tag);
}

void Workflow::Start() {
    if (actions_.empty()) {
        OnFinally();
        return;
    }

    // 取链表首部元素
    auto header = actions_.front();
    actions_.pop_front();

    std::string thread_name;
    AsyncWorkflowTask action;
    WorkflowActionType type;
    std::tie(thread_name, action, type) = header;

    // 执行首部元素
    if (type == WorkflowActionType::ACTION_TYPE_NORMAL) {
        // 普通任务
        auto runnable = [this, thread_name, action]() {
            if (action) {
                Notifier notifier = [this] {
                    this->Notify();
                };
                action(notifier);
            } else {
                Notify();
            }
        };
        if (exe_runnable_delegate_) {
            exe_runnable_delegate_(thread_name, runnable);
        } else {
            Threads::PostOnThread(thread_name, runnable, 0, TAG);
        }
    } else {
        // Fork 任务
        auto runnable = [thread_name, action] {
            if (action) {
                action([] {});
            }
        };
        if (exe_runnable_delegate_) {
            exe_runnable_delegate_(thread_name, runnable);
        } else {
            Threads::PostOnThread(thread_name, runnable, 0, TAG);
        }

        // 立即执行下一个
        Notify();
    }

}

void Workflow::Notify() {
    Start();
}

void Workflow::OnFinally() {
    if (finally_block_ == nullptr) {
        delete this;
        return;
    }

    std::string thread_name;
    WorkflowTask block;
    std::tie(thread_name, block) = (*finally_block_);
    delete finally_block_;
    finally_block_ = nullptr;

    auto runnable = [this, block] {
        if (block) {
            block();
        }
        delete this;
    };

    if (exe_runnable_delegate_) {
        exe_runnable_delegate_(thread_name, runnable);
    } else {
        Threads::PostOnThread(thread_name, runnable, 0, "Workflow.onFinally");
    }
}

void Workflow::Interrupt() {
    actions_.clear();
}

Workflow *Workflow::Async(const std::string &thread, const AsyncWorkflowTask &action, const std::string &tag) {
    auto task = [action, tag](Notifier notifier) {
        logger.i(TAG, "即将执行「%s」", tag.c_str());
        action(std::move(notifier));
        logger.i("Workflow", "执行「%s」完成", tag.c_str());
    };
    WorkflowTaskInfo _action(thread, task, WorkflowActionType::ACTION_TYPE_NORMAL);
    actions_.push_back(_action);
    return this;
}

Workflow *
Workflow::Async(const std::string &thread, std::function<void(Workflow *, Notifier)> action, const std::string &tag) {
    auto task = [this, action, tag](Notifier notifier) {
        logger.i(TAG, "即将执行「%s」", tag.c_str());
        action(this, std::move(notifier));
        logger.i("Workflow", "执行「%s」完成", tag.c_str());
    };
    WorkflowTaskInfo act(thread, task, WorkflowActionType::ACTION_TYPE_NORMAL);
    actions_.push_back(act);
    return this;
}

Workflow *Workflow::Finally(const std::string &thread, WorkflowTask action, const std::string &tag) {
    auto temp = [action, tag]() {
        logger.i(TAG, "即将执行「%s」", tag.c_str());
        action();
        logger.i("Workflow", "执行「%s」完成", tag.c_str());
    };
    delete finally_block_;
    finally_block_ = new std::tuple<std::string, WorkflowTask>(thread, temp);
    return this;
}

Workflow *
Workflow::Finally(const std::string &thread, const std::function<void(Workflow *)> &action, const std::string &tag) {
    auto temp = [this, action, tag]() {
        logger.i(TAG, "即将执行「%s」", tag.c_str());
        action(this);
        logger.i("Workflow", "执行「%s」完成", tag.c_str());
    };
    delete finally_block_;
    finally_block_ = new std::tuple<std::string, WorkflowTask>(thread, temp);
    return this;
}

Workflow *Workflow::Fork(const std::string &thread, WorkflowTask action, const std::string &tag) {
    // 将一个同步的 Action 转为 异步的。
    AsyncWorkflowTask block = [action, tag](const Notifier &notifier) {
        logger.i(TAG, "即将执行「%s」", tag.c_str());
        action();
        // 这里不再需要 notify
        logger.i("Workflow", "执行「%s」完成", tag.c_str());
    };

    // 加入到队列
    WorkflowTaskInfo wrapper(thread, block, WorkflowActionType::ACTION_TYPE_FORK);
    actions_.push_back(wrapper);
    return this;
}

Workflow *
Workflow::Fork(const std::string &thread, const std::function<void(Workflow *)> &action, const std::string &tag) {
    auto wrapper = [this, action]() {
        action(this);
    };
    Fork(thread, wrapper, tag);
    return this;
}

void Workflow::Set(const std::string &key, std::shared_ptr<void> obj) {
    data_cache_[key] = obj;
}
