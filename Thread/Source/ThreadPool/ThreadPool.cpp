//
// Created by lixiaoqing on 2021/8/18.
//

#include <sstream>
#include "ThreadPool/ThreadPool.h"
#include "Thread/ThreadLog.h"
#include "Lock/SmartRecursiveLock.h"


ThreadPool *ThreadPool::newThreadPool(std::string name, size_t maxThreadCount) {
    return newThreadPool(std::move(name), maxThreadCount, [] {});
}

ThreadPool *ThreadPool::newThreadPool(std::string name, size_t maxThreadCount, Runnable initialRunnable) {
    return new ThreadPool(std::move(name), maxThreadCount, std::move(initialRunnable));
}

ThreadPool::ThreadPool(std::string name, size_t maxThreadCount, Runnable initialRunnable) {
//    logger.i("ThreadPool", "<init>");
    mName = std::move(name);
    mIsRunning = true;
    mMaxThreadCount = maxThreadCount;
    mInitialRunnable = std::move(initialRunnable);
    mCreatedThreadCount = 0;
}

void ThreadPool::execute(std::string name, Runnable runnable) {
    // logger.i("ThreadPool", "execute(.) Called !");

    mTaskMutex.lock();

    // 添加到待执行任务的列表
    RunnableWrapper wrapper(std::move(name), std::move(runnable));
    mTasks.push(wrapper);

    mTaskMutex.unlock();

    // 尝试执行
    tryExecute();
}

void ThreadPool::tryExecute() {
//    logger.i("ThreadPool", "tryExecute() Called !");

    SmartRecursiveLock lock(mThreadMutex);

    // 如果有空闲的线程
    if (!mIdleThreads.empty()) {
        exeWithIdleThread();
        return;
    }

    // 线程总数已经达到最大值，等待
    if (mWorkingThreads.size() >= mMaxThreadCount) {
        return;
    }

    // 尚未达到最大值，创建新的线程
    createNewThread();
    exeWithIdleThread();
}

void ThreadPool::exeWithIdleThread() {
//    logger.i("ThreadPool", "exeWithIdleThread() Called !");

    if (!hasTask()) {
        return;
    }

    SmartRecursiveLock lockThreads(mTaskMutex);

    // 如果没有空闲的线程
    if (mIdleThreads.empty()) {
        return;
    }

    // 获取第一个空闲的线程
    auto *idleThread = mIdleThreads.at(0);

    // 获取之后从空闲缓存中移除
    auto it = mIdleThreads.begin();
    mIdleThreads.erase(it);

    // 将线程添加到工作线程组
    mWorkingThreads.push_back(idleThread);

    // 为选中的线程增加一个工作
    Runnable runnable = [this, idleThread] {
//        logger.i("ThreadPool",std::string("[").append(idleThread->getName()).append("]").append(" Run the Runnable !"));

        if (!hasTask()) {
            return;
        }

        mTaskMutex.lock();

        // 获取第一个任务
        RunnableWrapper task = mTasks.front();
        mTasks.pop();

        mTaskMutex.unlock();

        if (task.runnable) {
            {
                std::ostringstream oss;
                oss << "[" << idleThread->getName() << "] 即将运行任务 [" << task.name << "]";
//                logger.i("ThreadPool", oss.str());
            }

            // 执行 runnable
            task.runnable();

            {
                std::ostringstream oss;
                oss << "[" << idleThread->getName() << "] 任务 [" << task.name << "] 执行完毕！";
//                logger.i("ThreadPool", oss.str());
            }
        }

        {
            // 执行完后，把当前线程放回空闲数组中
            SmartRecursiveLock lock(mThreadMutex);
            mIdleThreads.push_back(idleThread);
            auto tempIt = mWorkingThreads.begin();
            while (tempIt != mWorkingThreads.end()) {
                if ((*tempIt) == idleThread) {
                    mWorkingThreads.erase(tempIt);
                    break;
                }
                tempIt++;
            }

            std::string msg("Task Finished! Idle Count = ");
            msg.append(std::to_string(mIdleThreads.size()));
            msg.append(", Working Count = ");
            msg.append(std::to_string(mWorkingThreads.size()));
//            logger.i("ThreadPool", msg);
        }

        // 再次触发执行下一个任务
        tryExecute();
    };
    idleThread->post(runnable);
}

void ThreadPool::createNewThread() {
//    logger.i("ThreadPool", "createNewThread() Called !");

    SmartRecursiveLock lockThreads(mThreadMutex);

    if (!mIsRunning) {
        logger.i("ThreadPool", "Thread Pool has Shut down! Give up Create New Thread! ");
        return;
    }

    // 创建线程
    auto *thread = SmartLoopThread::newThread();
    // 线程名
    std::string name;
    generateNextThreadName(name);
    thread->setName(name);

    // 添加到空闲线程组
    mIdleThreads.push_back(thread);

    // 开启线程
    thread->start();
    thread->post(mInitialRunnable);
}

void ThreadPool::generateNextThreadName(std::string &name) {
//    logger.i("ThreadPool", "generateNextThreadName() Called !");

    SmartRecursiveLock lock(mThreadMutex);

    mCreatedThreadCount++;
    name = std::string(mName) + "-T" + std::to_string(mCreatedThreadCount);

//    logger.i("ThreadPool", std::string("New Thread's Name:") + name);
}

bool ThreadPool::hasTask() {
//    logger.i("ThreadPool", "hasTask() Called !");

    SmartRecursiveLock lockTasks(mTaskMutex);
    return !mTasks.empty();
}

void ThreadPool::releaseIdleThreads() {
    SmartRecursiveLock lock(mThreadMutex);

    std::string log = std::string("--> ")
            .append(std::to_string(mIdleThreads.size()))
            .append(" idle threads will be Deleted. ");
//    logger.i("ThreadPool", log.c_str())

    auto it = mIdleThreads.begin();
    while (it != mIdleThreads.end()) {
        (*it)->tryStop();
        it = mIdleThreads.erase(it);
    }

    mIdleThreads.clear();
}

void ThreadPool::shutDownAndDeleteSelf() {
    mThreadMutex.lock();
//    logger.i("ThreadPool", "shutDownAndDeleteSelf() called !");

    // 释放所有的空闲线程
    releaseIdleThreads();

    // 释放所有正在工作的线程
    auto it = mWorkingThreads.begin();
    while (it != mWorkingThreads.end()) {
        (*it)->tryStop();
        it = mWorkingThreads.erase(it);
    }

    mWorkingThreads.clear();

    mIsRunning = false;

    mThreadMutex.unlock();

    delete this;
}

size_t ThreadPool::getThreadCount() {
    SmartRecursiveLock lock(mThreadMutex);
    return mIdleThreads.size() + mWorkingThreads.size();
}