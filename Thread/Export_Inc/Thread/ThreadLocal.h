//
// Created by lixiaoqing on 2021/8/12.
//

#ifndef THREAD_THREADLOCAL_H
#define THREAD_THREADLOCAL_H


#include <thread>


/**
 * 类似于 Java 的 ThreadLocal 的实现。
 * @tparam T
 */
template<class T>
class ThreadLocal {

public:

    ThreadLocal() {
        token = 0;
        pthread_key_create(&token, nullptr);
    }

    T *get() {
        return (T *) pthread_getspecific(token);
    }

    void set(T *data) {
        pthread_setspecific(token, data);
    }

private:

    pthread_key_t token;

};


#endif //THREAD_THREADLOCAL_H
