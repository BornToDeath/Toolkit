//
// Created by lixiaoqing on 2021/9/3.
//

#ifndef NOTIFICATIONCENTER_NOTIFICATIONCENTER_H
#define NOTIFICATIONCENTER_NOTIFICATIONCENTER_H

#include <iostream>
#include <functional>
#include <mutex>
#include <vector>
#include <map>


using NotificationObserver = std::function<void(void *data)>;
using ObserverVector = std::vector<NotificationObserver>;


/**
 * 通知中心
 */
class NotificationCenter {

public:

    NotificationCenter();

    ~NotificationCenter();

public:

    /**
     * 添加一个观察者，用于观察指定 name 的通知
     * @param name
     * @param observer
     */
    void addObserver(const std::string &name, const NotificationObserver &observer);

    /**
     * 移除观察者 observer 对指定 name 的监听
     * @param name
     * @param observer
     */
    void removeObserver(const std::string &name, const NotificationObserver &observer);

    /**
     * 发出一条通知
     * @param name
     * @param data
     */
    void postNotification(const std::string &name, void *data);

public:

    /**
     * 默认的消息通知中心
     */
    static NotificationCenter *defaultCenter;

private:

    /**
     * 观察者列表
     */
    std::map<std::string, ObserverVector *> *observers;

    /**
     * 保证内部线程安全的锁
     */
    std::recursive_mutex mutex;
};


#endif //NOTIFICATIONCENTER_NOTIFICATIONCENTER_H
