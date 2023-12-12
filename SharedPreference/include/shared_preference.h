//
// Created by lixiaoqing on 2021/9/15.
//

#ifndef SHAREDPREFERENCE_SHARED_PREFERENCE_H
#define SHAREDPREFERENCE_SHARED_PREFERENCE_H

#include <functional>
#include <mutex>

#include "db_master.h"


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
    static bool Init(DbMaster *master);

    /**
     * 获取一个 SharedPreference 实例
     * @param name
     * @return
     */
    static SharedPreference *GetSharedPreference(const std::string &name);

    /**
     * 这个方法把获取 sp 对象和使用完释放 sp 对象封装起来了。
     * 等价于：
     * auto sp = SharedPreference::get(name);
     * block(sp);
     * sp->release();
     */
    static void With(const std::string &name, const std::function<void(SharedPreference *)> &block);

    static unsigned long long CurrentTimeMills();

    bool GetBool(const std::string &key, bool default_value);

    int GetInt(const std::string &key, int default_value);

    float GetFloat(const std::string &key, float default_value);

    double GetDouble(const std::string &key, double default_value);

    long long GetLongLong(const std::string &key, long long default_value);

    std::string GetString(const std::string &key, const std::string &default_value);

    bool SetBool(const std::string &key, bool value);

    bool SetInt(const std::string &key, int value);

    bool SetFloat(const std::string &key, float value);

    bool SetDouble(const std::string &key, double value);

    bool SetLongLong(const std::string &key, long long value);

    bool SetString(const std::string &key, const std::string &value);

    /**
     * 释放当前的 SP 对象。
     * 因为同一个 SP 对象可能被多个地方使用，所以不能直接调用 delete sp;
     * 每个 sp 对象都有一个引用计数，调用这个方法后，引用计数 -1。 如果减到 0，会自动释放。
     * 调用该方法后，不能再使用这个指针了。
     *
     * @return 内部是否调用了 delete。
     */
    bool Release();

    /**
     * 引用计数 +1.
     */
    void Retain();

    /** 这个方法不应该用于业务，只用于日常调试 */
    unsigned long long GetRefCount() const;

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

    static DbMaster *database_;

    static std::vector<SharedPreference *> sp_list_;

    static std::recursive_mutex sp_list_mutex_;

    std::string name_;

    /**
     * 引用计数
     */
    unsigned long long ref_count_ = 0;

    std::recursive_mutex ref_count_mutex_;
};


#endif //SHAREDPREFERENCE_SHARED_PREFERENCE_H
