//
// Created by lixiaoqing on 2021/8/18.
//

#include "thread/thread_pool.h"

#include <sstream>

#include "lock/smart_recursive_lock.h"
#include "thread/thread_log.h"


ThreadPool *ThreadPool::NewInstance(std::string name, const size_t maxThreadCount) {
    return NewInstance(std::move(name), maxThreadCount, [] {});
}

ThreadPool *ThreadPool::NewInstance(std::string name, const size_t maxThreadCount, Runnable initialRunnable) {
    return new ThreadPool(std::move(name), maxThreadCount, std::move(initialRunnable));
}

ThreadPool::ThreadPool(std::string name, size_t maxThreadCount, Runnable initialRunnable) {
//    logger.i("ThreadPool", "<init>");
    name_ = std::move(name);
    is_running_ = true;
    max_thread_count_ = maxThreadCount;
    init_runnable_ = std::move(initialRunnable);
    created_thread_count_ = 0;
}

void ThreadPool::Execute(std::string name, Runnable runnable) {
    // logger.i("ThreadPool", "execute(.) Called !");

    task_mutex_.lock();

    // 添加到待执行任务的列表
    RunnableWrapper wrapper(std::move(runnable), std::move(name));
    tasks_.push(wrapper);

    task_mutex_.unlock();

    // 尝试执行
    TryExecute();
}

void ThreadPool::TryExecute() {
//    logger.i("ThreadPool", "tryExecute() Called !");

    SmartRecursiveLock lock(thread_mutex_);

    // 如果有空闲的线程
    if (!idle_threads_.empty()) {
        ExecuteWithIdleThread();
        return;
    }

    // 线程总数已经达到最大值，等待
    if (working_threads_.size() >= max_thread_count_) {
        return;
    }

    // 尚未达到最大值，创建新的线程
    CreateNewThread();
    ExecuteWithIdleThread();
}

void ThreadPool::ExecuteWithIdleThread() {
//    logger.i("ThreadPool", "exeWithIdleThread() Called !");

    if (!HasTask()) {
        return;
    }

    SmartRecursiveLock lockThreads(task_mutex_);

    // 如果没有空闲的线程
    if (idle_threads_.empty()) {
        return;
    }

    // 获取第一个空闲的线程
    auto *idle_thread = idle_threads_.at(0);

    // 获取之后从空闲缓存中移除
    auto it = idle_threads_.begin();
    idle_threads_.erase(it);

    // 将线程添加到工作线程组
    working_threads_.push_back(idle_thread);

    // 为选中的线程增加一个工作
    Runnable runnable = [this, idle_thread] {
//        logger.i("ThreadPool",std::string("[").append(idleThread->getName()).append("]").append(" Run the Runnable !"));

        if (!HasTask()) {
            return;
        }

        task_mutex_.lock();

        // 获取第一个任务
        RunnableWrapper task = tasks_.front();
        tasks_.pop();

        task_mutex_.unlock();

        if (task.GetRunnable()) {
            // 执行 runnable
            task.GetRunnable()();
        }

        {
            // 执行完后，把当前线程放回空闲数组中
            SmartRecursiveLock lock(thread_mutex_);
            idle_threads_.push_back(idle_thread);
            auto tmp_thread = working_threads_.begin();
            while (tmp_thread != working_threads_.end()) {
                if ((*tmp_thread) == idle_thread) {
                    working_threads_.erase(tmp_thread);
                    break;
                }
                tmp_thread++;
            }

            std::string msg("Task Finished! Idle Count = ");
            msg.append(std::to_string(idle_threads_.size()));
            msg.append(", Working Count = ");
            msg.append(std::to_string(working_threads_.size()));
//            logger.i("ThreadPool", msg);
        }

        // 再次触发执行下一个任务
        TryExecute();
    };
    idle_thread->Post(runnable);
}

void ThreadPool::CreateNewThread() {
//    logger.i("ThreadPool", "createNewThread() Called !");

    SmartRecursiveLock lockThreads(thread_mutex_);

    if (!is_running_) {
        logger.i("ThreadPool", "Thread Pool has Shut down! Give up Create New Thread! ");
        return;
    }

    // 创建线程
    auto *thread = SmartLoopThread::NewInstance();
    // 线程名
    std::string name;
    GenerateNextThreadName(name);
    thread->SetName(name);

    // 添加到空闲线程组
    idle_threads_.push_back(thread);

    // 开启线程
    thread->Start();
    thread->Post(init_runnable_);
}

void ThreadPool::GenerateNextThreadName(std::string &name) {
//    logger.i("ThreadPool", "generateNextThreadName() Called !");

    SmartRecursiveLock lock(thread_mutex_);

    created_thread_count_++;
    name = std::string(name_) + "-T" + std::to_string(created_thread_count_);

//    logger.i("ThreadPool", std::string("New Thread's Name:") + name);
}

bool ThreadPool::HasTask() {
//    logger.i("ThreadPool", "hasTask() Called !");

    SmartRecursiveLock lock(task_mutex_);
    return !tasks_.empty();
}

void ThreadPool::ReleaseIdleThreads() {
    SmartRecursiveLock lock(thread_mutex_);

    std::string log = std::string("--> ")
            .append(std::to_string(idle_threads_.size()))
            .append(" idle threads will be Deleted. ");
//    logger.i("ThreadPool", log.c_str())

    auto it = idle_threads_.begin();
    while (it != idle_threads_.end()) {
        (*it)->TryStop();
        it = idle_threads_.erase(it);
    }

    idle_threads_.clear();
}

void ThreadPool::ShutDownAndDeleteSelf() {
    thread_mutex_.lock();
//    logger.i("ThreadPool", "shutDownAndDeleteSelf() called !");

    // 释放所有的空闲线程
    ReleaseIdleThreads();

    // 释放所有正在工作的线程
    auto it = working_threads_.begin();
    while (it != working_threads_.end()) {
        (*it)->TryStop();
        it = working_threads_.erase(it);
    }

    working_threads_.clear();

    is_running_ = false;

    thread_mutex_.unlock();

    delete this;
}

size_t ThreadPool::GetThreadCount() {
    SmartRecursiveLock lock(thread_mutex_);
    return idle_threads_.size() + working_threads_.size();
}