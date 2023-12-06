//
// Created by lixiaoqing on 2021/8/12.
//

#ifndef THREAD_RUNNABLE_H
#define THREAD_RUNNABLE_H


#include <functional>
#include <utility>

typedef std::function<void(void)> Runnable;

///**
// * 判断两个 Runnable 对象是否相等
// */
//bool isSameRunnable(Runnable r1, Runnable r2) {
//    auto p1 = r1.target<void(*)(void)>();
//    auto p2 = r2.target<void(*)(void)>();
//    return p1 == p2;
//}


class RunnableWrapper {

public:
    RunnableWrapper(Runnable runnable, std::string name) : runnable_(std::move(runnable)), name_(std::move(name)) {}

public:
    Runnable GetRunnable() const {
        return runnable_;
    }

    std::string GetName() const {
        return name_;
    }

private:
    Runnable runnable_;
    std::string name_;
};


#endif //THREAD_RUNNABLE_H
