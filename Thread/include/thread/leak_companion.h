//
// Created by lixiaoqing on 2021/8/12.
//

#ifndef THREAD_LEAK_COMPANION_H
#define THREAD_LEAK_COMPANION_H


#include <mutex>
#include <vector>

#define ADD_LEAK_DETECTOR LeakCompanion companion_;


/**
 * 这个类是用来检测内存泄漏的。
 *
 * 作为要检测的类的伴生对象。
 * 当创建时，静态计数 +1，析构时 -1。
 */
class LeakCompanion {

public:

    LeakCompanion();

    explicit LeakCompanion(const std::string &name);

    ~LeakCompanion();

public:

    /**
     * 获取当前伴生对象的数量
     */
    static size_t GetCurrentCount();

    static void AssertCount(size_t size);

    void SetName(std::string name);

private:

    static std::vector<LeakCompanion *> companions_;

    static std::recursive_mutex mutex_;

    std::string name_;

};


#endif //THREAD_LEAK_COMPANION_H
