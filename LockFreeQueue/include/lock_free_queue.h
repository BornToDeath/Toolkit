//
// Created by lixiaoqing on 2022/10/25.
//

#ifndef LOCKFREEQUEUE_H
#define LOCKFREEQUEUE_H

#include <cstring>

#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
//#include <sys/stat.h>
//#include <sys/types.h>


/* 开启 Spinlock 锁，适用多生产者多消费者场景 */
#define USE_LOCK

/* 开启队列大小的2的幂对齐。目的是方便计算头尾索引 */
//#define USE_POT

/* 启 memory barrier */
//#define USE_MEM_BAR

/* 判断 x 是否是 2 的幂 */
#define IS_POT(x) ((x) && !((x) & ((x) - 1)))

/* 开启内存屏障 */
#define MEMORY_BARRIER __sync_synchronize()

/**
 * 无锁队列
 * 多线程场景下，需要定义 USE_LOCK 宏，开启锁保护。
 * @tparam T
 */
template<typename T>
class LockFreeQueue {

public:

    explicit LockFreeQueue(unsigned int size, const char *name = nullptr) {

#ifdef USE_POT
        if (!IS_POT(size)) {
            size = roundup_pow_of_two(size);
        }
#endif

        size_ = size;
        if (name == nullptr) {
            buffer_ = new T[size_]();
        } else {
            // 创建共享内存信道
            int shm_fd = shm_open(name, O_CREAT | O_RDWR, 0666);
            if (shm_fd < 0) {
                perror("shm_open failed");
                exit(1);
            }
            // 设置共享内存的大小
            if (ftruncate(shm_fd, size_ * sizeof(T)) < 0) {
                perror("ftruncate failed");
                close(shm_fd);
                exit(1);
            }
            // 共享内存映射
            void *addr = mmap(nullptr, size_ * sizeof(T), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
            if (addr == MAP_FAILED) {
                perror("mmap failed");
                close(shm_fd);
                exit(1);
            }
            if (close(shm_fd) == -1) {
                perror("close failed");
                exit(1);
            }
            buffer_ = static_cast<T *>(addr);
            memcpy(shm_name_, name, strlen(name));
        }
    }

    ~LockFreeQueue() {
        if (shm_name_[0] == 0) {  // 创建的是堆内存
            delete[] buffer_;
        } else {  // 创建的是共享内存
            if (munmap(buffer_, size_ * sizeof(T)) == -1) {
                perror("munmap failed");
            }
            if (shm_unlink(shm_name_) == -1) {
                perror("shm_unlink failed");
            }
        }
        buffer_ = nullptr;
    }

public:

    bool Push(const T &value) {
#ifdef USE_LOCK
        lock_.Lock();
#endif
        if (IsFull()) {
#ifdef USE_LOCK
            lock_.Unlock();
#endif
            return false;
        }

        memcpy(buffer_ + tail_, &value, sizeof(T));

#ifdef USE_MEM_BAR
        MEMORY_BARRIER;
#endif

#ifdef USE_POT
        tail_ = (tail_ + 1) & (size_ - 1);
#else
        tail_ = (tail_ + 1) % size_;
#endif

#ifdef USE_LOCK
        lock_.Unlock();
#endif

        return true;
    }

    bool Pop(T &value) {
#ifdef USE_LOCK
        lock_.Lock();
#endif
        if (IsEmpty()) {
#ifdef USE_LOCK
            lock_.Unlock();
#endif
            return false;
        }

        memcpy(&value, buffer_ + head_, sizeof(T));

#ifdef USE_MEM_BAR
        MEMORY_BARRIER;
#endif

#ifdef USE_POT
        head_ = (head_ + 1) & (size_ - 1);
#else
        head_ = (head_ + 1) % size_;
#endif

#ifdef USE_LOCK
        lock_.Unlock();
#endif
        return true;
    }

    bool IsFull() const {
#ifdef USE_POT
        return head_ == ((tail_ + 1) & (size_ - 1));
#else
        return head_ == (tail_ + 1) % size_;
#endif
    }

    bool IsEmpty() const {
        return head_ == tail_;
    }

    unsigned int Head() const {
        return head_;
    }

    unsigned int Tail() const {
        return tail_;
    }

private:

    /**
     * 计算大于 size 的第一个 2 的幂。如 size = 10, 则返回 16
     * @param size
     * @return
     */
    unsigned int roundup_pow_of_two(size_t size) {
        size |= size >> 1;
        size |= size >> 2;
        size |= size >> 4;
        size |= size >> 8;
        size |= size >> 16;
        size |= size >> 32;
        return size + 1;
    }

private:
    class Spinlock {
    public:
        void Lock() {
            while (!__sync_bool_compare_and_swap(&m_lock_, 0, 1)) {}
        }

        void Unlock() {
            __sync_lock_release(&m_lock_);
        }

    private:
        int m_lock_{0};
    };

private:
    char shm_name_[32]{};              // 共享内存 key 的路径名称
    volatile unsigned int head_{0};  // 队列头部索引
    volatile unsigned int tail_{0};  // 队列尾部索引（指向队列尾部元素的下一个元素）
    unsigned int size_{0};           // 队列大小
    T *buffer_{nullptr};             // 数据缓冲区
#ifdef USE_LOCK
    Spinlock lock_;                  // CAS 实现的锁机制
#endif
};

#endif //LOCKFREEQUEUE_H
