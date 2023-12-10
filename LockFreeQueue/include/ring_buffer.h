//
// Created by lixiaoqing on 2022/10/25.
//

#ifndef RINGBUFFER_H
#define RINGBUFFER_H

/**
 * 无锁环形缓冲区
 * 仅适用于单生产者单消费者场景。
 * @tparam T
 */
template<typename T>
class RingBuffer {
public:
    explicit RingBuffer(unsigned int size) : size_(size + 1), head_(0), tail_(0) {
        if (size <= 0) {
            throw "size can not less than 0";
        }
        data_ = new T[size_]();
    }

    ~RingBuffer() {
        delete[] data_;
        data_ = nullptr;
    }

public:
    bool Push(const T &value) {
        if (IsFull()) {
            return false;
        }
        data_[tail_] = value;
        tail_ = (tail_ + 1) % size_;
        return true;
    }

    T Pop() {
        if (IsEmpty()) {
            throw "ring buffer is empty";
        }
        auto value = data_[head_];
        head_ = (head_ + 1) % size_;
        return value;
    }

    bool IsFull() const {
        return head_ == (tail_ + 1) % size_;
    }

    bool IsEmpty() const {
        return head_ == tail_;
    }

    T *Head() const {
        if (IsEmpty()) {
            return nullptr;
        }
        return data_ + head_;
    }

    T *Tail() const {
        if (IsEmpty()) {
            return nullptr;
        }
        if (tail_ == 0) {
            return data_ + size_ - 1;
        }
        return data_ + tail_ - 1;
    }

    unsigned int Capacity() const {
        return size_ - 1;
    }

private:
    const unsigned int size_;  // 队列长度
    int head_;                 // 队列头部索引
    int tail_;                 // 队列尾部索引
    T *data_;                  // 数据缓冲区
};

#endif