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
    explicit RingBuffer(unsigned int size) : m_size(size + 1), m_head(0), m_tail(0) {
        if (size <= 0) {
            throw "size can not less than 0";
        }
        m_data = new T[m_size]();
    }

    ~RingBuffer() {
        delete[] m_data;
        m_data = nullptr;
    }

public:
    bool push(const T &value) {
        if (isFull()) {
            return false;
        }
        m_data[m_tail] = value;
        m_tail = (m_tail + 1) % m_size;
        return true;
    }

    T pop() {
        if (isEmpty()) {
            throw "ring buffer is empty";
        }
        auto value = m_data[m_head];
        m_head = (m_head + 1) % m_size;
        return value;
    }

    bool isFull() const {
        return m_head == (m_tail + 1) % m_size;
    }

    bool isEmpty() const {
        return m_head == m_tail;
    }

    T *head() const {
        if (isEmpty()) {
            return nullptr;
        }
        return m_data + m_head;
    }

    T *tail() const {
        if (isEmpty()) {
            return nullptr;
        }
        if (m_tail == 0) {
            return m_data + m_size - 1;
        }
        return m_data + m_tail - 1;
    }

    unsigned int capacity() const {
        return m_size - 1;
    }

private:
    const unsigned int m_size;  // 队列长度
    int m_head;                 // 队列头部索引
    int m_tail;                 // 队列尾部索引
    T *m_data;                  // 数据缓冲区
};

#endif