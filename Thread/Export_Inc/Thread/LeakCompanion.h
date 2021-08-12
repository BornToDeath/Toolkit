//
// Created by lixiaoqing on 2021/8/12.
//

#ifndef THREAD_LEAKCOMPANION_H
#define THREAD_LEAKCOMPANION_H


#include <vector>
#include <mutex>

#define ADD_LEAK_DETECTOR LeakCompanion __companion;


/**
 * 这个类是用来检测内存泄漏的。
 *
 * 作为要检测的类的伴生对象。
 * 当创建时，静态计数 +1，析构时 -1。
 */
class LeakCompanion {

public:

    LeakCompanion();

    explicit LeakCompanion(std::string name);

    ~LeakCompanion();

public:

    /**
     * 获取当前伴生对象的数量
     */
    static size_t getCurrentCount();

    static void assertCount(size_t size);

    void setName(std::string name);

private:

    static std::vector<LeakCompanion *> all;

    static std::recursive_mutex mutex;

private:

    std::string name;

};


#endif //THREAD_LEAKCOMPANION_H
