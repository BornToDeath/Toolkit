//
// Created by lixiaoqing on 2021/9/3.
//

#include "notification_center.h"
#include "lock/smart_recursive_lock.h"


NotificationCenter *NotificationCenter::default_center_ = new NotificationCenter();


NotificationCenter::NotificationCenter() {
    observers_ = new std::map<std::string, ObserverVector *>();
}

NotificationCenter::~NotificationCenter() {
    for (auto &observer: *observers_) {
        auto v = observer.second;
        delete v;
    }
    delete observers_;
    observers_ = nullptr;
}

void NotificationCenter::AddObserver(const std::string &name, const NotificationObserver &observer) {
    // 保证线程安全
    SmartRecursiveLock lock(mutex_);

    // 获取 name 对应的观察者列表
    ObserverVector *observer_vector = nullptr;
    auto it = this->observers_->find(name);
    if (it != this->observers_->end()) {
        observer_vector = it->second;
    }

    if (observer_vector == nullptr) {
        observer_vector = new ObserverVector();
        this->observers_->insert(std::make_pair(name, observer_vector));
    }

    // 将观察者插入到观察者列表中
    observer_vector->push_back(observer);
}

void NotificationCenter::RemoveObserver(const std::string &name, const NotificationObserver &observer) {
    // 保证线程安全
    SmartRecursiveLock lock(mutex_);

    // 获取 name 对应的观察者列表
    auto it = this->observers_->find(name);
    if (it == this->observers_->end()) {
        return;
    }

    // 移除 name 对应的观察者
    auto observer_vector = it->second;
    for (auto it2 = observer_vector->begin(); it2 != observer_vector->end(); ++it2) {
        if (it2->target<NotificationObserver>() == observer.target<NotificationObserver>()) {
            observer_vector->erase(it2);
            return;
        }
    }
}

void NotificationCenter::PostNotification(const std::string &name, void *data) {

    ObserverVector list;

    {
        // 保证线程安全
        SmartRecursiveLock lock(mutex_);

        // 获取 name 对应的观察者列表
        auto it = this->observers_->find(name);
        if (it == this->observers_->end()) {
            return;
        }

        auto observer_vector = it->second;
        if (observer_vector == nullptr) {
            return;
        }
        if (observer_vector->empty()) {
            return;
        }

        for (auto &it2: *observer_vector) {
            list.push_back(it2);
        }
    }

    // 处理消息
    for (auto &observer: list) {
        observer(data);
    }
}