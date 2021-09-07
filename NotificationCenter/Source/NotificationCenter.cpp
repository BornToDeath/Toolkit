//
// Created by lixiaoqing on 2021/9/3.
//

#include "NotificationCenter.h"
#include "Lock/SmartRecursiveLock.h"


NotificationCenter *NotificationCenter::defaultCenter = new NotificationCenter();


NotificationCenter::NotificationCenter() {
    observers = new std::map<std::string, ObserverVector *>();
}

NotificationCenter::~NotificationCenter() {
    for (auto &observer : *observers) {
        auto v = observer.second;
        delete v;
    }
    delete observers;
    observers = nullptr;
}

void NotificationCenter::addObserver(const std::string &name, const NotificationObserver &observer) {
    // 保证线程安全
    SmartRecursiveLock lock(mutex);

    // 获取 name 对应的观察者列表
    ObserverVector *observerVector = nullptr;
    auto it = this->observers->find(name);
    if (it != this->observers->end()) {
        observerVector = it->second;
    }

    if (observerVector == nullptr) {
        observerVector = new ObserverVector();
        this->observers->insert(std::make_pair(name, observerVector));
    }

    // 将观察者插入到观察者列表中
    observerVector->push_back(observer);
}

void NotificationCenter::removeObserver(const std::string &name, const NotificationObserver &observer) {
    // 保证线程安全
    SmartRecursiveLock lock(mutex);

    // 获取 name 对应的观察者列表
    auto it = this->observers->find(name);
    if (it == this->observers->end()) {
        return;
    }

    // 移除 name 对应的观察者
    auto observerVector = it->second;
    for (auto it2 = observerVector->begin(); it2 != observerVector->end(); ++it2) {
        if (it2->target<NotificationObserver>() == observer.target<NotificationObserver>()) {
            observerVector->erase(it2);
            return;
        }
    }
}

void NotificationCenter::postNotification(const std::string &name, void *data) {

    ObserverVector list;

    {
        // 保证线程安全
        SmartRecursiveLock lock(mutex);

        // 获取 name 对应的观察者列表
        auto it = this->observers->find(name);
        if (it == this->observers->end()) {
            return;
        }

        auto observerVector = it->second;
        if (observerVector == nullptr) {
            return;
        }
        if (observerVector->empty()) {
            return;
        }

        for (auto &it2 : *observerVector) {
            list.push_back(it2);
        }
    }

    // 处理消息
    for (auto &observer: list) {
        observer(data);
    }
}