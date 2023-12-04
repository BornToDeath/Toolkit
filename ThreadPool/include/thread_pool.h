//
// Created by lixiaoqing on 2022/4/27.
//

#ifndef THREADPOOL_THREADPOOL_H
#define THREADPOOL_THREADPOOL_H

#include <vector>
#include <queue>
#include <functional>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <future>
#include <stdexcept>

class ThreadPool final {
public:
    explicit ThreadPool(int);

    ~ThreadPool();

    template<class F, class... Args>
    auto enqueue(F &&f, Args &&... args) -> std::future<typename std::result_of<F(Args...)>::type>;

private:
    std::vector<std::thread> workers_;
    std::queue<std::function<void()>> tasks_;
    std::mutex queue_mutex_;
    std::condition_variable condition_;
    std::atomic<bool> is_stop_{false};
    std::atomic<bool> is_init_{false};

private:
    // for test
    friend class ThreadPoolTest_ThreadPool2_Test;
};

inline ThreadPool::ThreadPool(int count) {
    if (count <= 0 || is_init_) {
        return;
    }

    for (auto i = 0; i < count; ++i) {
        auto runnable = [this]() {
            for (;;) {
                std::function<void()> task;
                {
                    std::unique_lock<std::mutex> lock(queue_mutex_);
                    condition_.wait(lock, [this]() { return !tasks_.empty() || is_stop_.load(); });
                    if (is_stop_.load() && tasks_.empty()) {
                        return;
                    }
                    task = std::move(tasks_.front());
                    tasks_.pop();
                }
                task();
            }
        };
        workers_.emplace_back(runnable);
    }
    is_init_.store(true);
}

inline ThreadPool::~ThreadPool() {
    is_stop_.store(true);
    condition_.notify_all();
    for (auto &worker: workers_) {
        worker.join();
    }
    is_init_.store(false);
}

template<class F, class ...Args>
auto ThreadPool::enqueue(F &&f, Args &&... args) -> std::future<typename std::result_of<F(Args...)>::type> {
    if (is_stop_ || !is_init_) {
        return {};
    }
    using return_type = typename std::result_of<F(Args...)>::type;
    auto task = std::make_shared<std::packaged_task<return_type()>>(
            std::bind(std::forward<F>(f), std::forward<Args>(args)...));
    std::future<return_type> res = task->get_future();
    {
        std::unique_lock<std::mutex> lock(queue_mutex_);
        if (is_stop_.load()) {
            throw std::runtime_error("enqueue on stopped ThreadPool");
        }
        tasks_.emplace([task]() { (*task)(); });
    }
    condition_.notify_one();
    return res;
}

#endif //THREADPOOL_THREADPOOL_H
