//
// Created by lixiaoqing on 2021/9/3.
//

#ifndef NOTIFICATIONCENTER_NOTIFICATION_CENTER_H
#define NOTIFICATIONCENTER_NOTIFICATION_CENTER_H

#include <functional>
#include <iostream>
#include <map>
#include <mutex>
#include <vector>


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
    void AddObserver(const std::string &name, const NotificationObserver &observer);

    /**
     * 移除观察者 observer 对指定 name 的监听
     * @param name
     * @param observer
     */
    void RemoveObserver(const std::string &name, const NotificationObserver &observer);

    /**
     * 发出一条通知
     * @param name
     * @param data
     */
    void PostNotification(const std::string &name, void *data);

public:

    /**
     * 默认的消息通知中心
     */
    static NotificationCenter *default_center_;

private:

    /**
     * 观察者列表
     */
    std::map<std::string, ObserverVector *> *observers_;

    /**
     * 保证内部线程安全的锁
     */
    std::recursive_mutex mutex_;
};


#endif //NOTIFICATIONCENTER_NOTIFICATION_CENTER_H
