//
// Created by lixiaoqing on 2021/9/15.
//

#ifndef SHAREDPREFERENCE_SHAREDPREFERENCE_H
#define SHAREDPREFERENCE_SHAREDPREFERENCE_H

#include <functional>
#include <mutex>
#include "DBMaster.h"


/**
 * 模仿安卓的 SharedPreference
 */
class SharedPreference {

public:

    /**
     * 初始化
     * @param master
     * @return
     */
    static bool init(DBMaster *master);

    /**
     * 获取一个 SharedPreference 实例
     * @param name
     * @return
     */
    static SharedPreference *getSharedPreference(const std::string &name);

    /**
     * 这个方法把获取 sp 对象和使用完释放 sp 对象封装起来了。
     * 等价于：
     * auto sp = SharedPreference::get(name);
     * block(sp);
     * sp->release();
     */
    static void with(const std::string &name, const std::function<void(SharedPreference *)>& block);

    static unsigned long long currentTimeMills();

    bool getBool(const std::string &key, bool defValue);

    int getInt(const std::string &key, int defValue);

    float getFloat(const std::string &key, float defValue);

    double getDouble(const std::string &key, double defValue);

    long long getLongLong(const std::string &key, long long defValue);

    std::string getString(const std::string &key, const std::string &defValue);

    bool setBool(const std::string &key, bool value);

    bool setInt(const std::string &key, int value);

    bool setFloat(const std::string &key, float value);

    bool setDouble(const std::string &key, double value);

    bool setLongLong(const std::string &key, long long value);

    bool setString(const std::string &key, const std::string &value);

    /**
     * 释放当前的 SP 对象。
     * 因为同一个 SP 对象可能被多个地方使用，所以不能直接调用 delete sp;
     * 每个 sp 对象都有一个引用计数，调用这个方法后，引用计数 -1。 如果减到 0，会自动释放。
     * 调用该方法后，不能再使用这个指针了。
     *
     * @return 内部是否调用了 delete。
     */
    bool release();

    /**
     * 引用计数 +1.
     */
    void retain();

    /** 这个方法不应该用于业务，只用于日常调试 */
    unsigned long long getRefCount() const;

private:

    /**
     * 私有构造
     */
    SharedPreference() = default;

    /**
     * 私有析构，在内部析构
     * 外部调用 release 方法释放
     */
    ~SharedPreference() = default;

private:

    static DBMaster *database;

    static std::vector<SharedPreference *> spList;

    static std::recursive_mutex staticMutex;

    std::string name;

    std::recursive_mutex mutex;

    /**
     * 引用计数
     */
    unsigned long long refCount = 0;
};


#endif //SHAREDPREFERENCE_SHAREDPREFERENCE_H
