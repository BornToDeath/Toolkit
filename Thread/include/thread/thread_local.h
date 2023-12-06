//
// Created by lixiaoqing on 2021/8/12.
//

#ifndef THREAD_THREAD_LOCAL_H
#define THREAD_THREAD_LOCAL_H


#include <thread>


/**
 * 类似于 Java 的 ThreadLocal 的实现。
 * @tparam T
 */
template<class T>
class ThreadLocal {

public:

    ThreadLocal() {
        token_ = 0;
        pthread_key_create(&token_, nullptr);
    }

    T *Get() {
        return (T *) pthread_getspecific(token_);
    }

    void Set(T *data) {
        pthread_setspecific(token_, data);
    }

private:

    pthread_key_t token_;

};


#endif //THREAD_THREAD_LOCAL_H
