//
// Created by lixiaoqing on 2021/8/12.
//

#ifndef THREAD_RUNNABLE_H
#define THREAD_RUNNABLE_H


#include <functional>

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

    RunnableWrapper(std::string name, Runnable runnable) {
        this->runnable = runnable;
        this->name = name;
    }

public:

    Runnable runnable;
    std::string name;
};


#endif //THREAD_RUNNABLE_H
