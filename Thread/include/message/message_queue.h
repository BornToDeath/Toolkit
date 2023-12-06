//
// Created by lixiaoqing on 2021/8/13.
//

#ifndef THREAD_MESSAGE_QUEUE_H
#define THREAD_MESSAGE_QUEUE_H


#include <condition_variable>

#include "message.h"


/**
 * 消息队列
 */
class MessageQueue {

public:

    MessageQueue();

    ~MessageQueue();

    /**
     * 添加一个消息到消息队列
     */
    void EnqueueMessage(Message *message, Timestamp when);

    /**
     * 获取一条消息。
     * 这个方法会阻塞，只有在退出时才会返回 nullptr
     * @return
     */
    Message *Next();

    /**
     * 退出消息循环
     * @param is_safely 是否安全的退出
     *                 true : 把所有在当前时刻之前应该执行的消息执行完再退出
     *                 false: 移除所有消息，即使存在还未执行的消息
     */
    void Quit(bool is_safely = true);

    /**
     * 设置当前 queue 所属线程的名称
     * @param name
     */
    void SetName(const std::string &name);

private:

    /**
     * 等待
     * @param wait_mills
     */
    void PollOnce(long long wait_mills);

    /**
     * 接收消息的通知
     */
    void Awake();

    /**
     * 移除队列中的所有将来才会执行的消息
     * 有可能队列中积压了到时间了但还没有执行的消息，这种消息等待其执行完毕
     */
    void RemoveAllMessagesSafely();

    /**
     * 移除队列中的所有消息。
     * 直接移除，无需关心消息是否执行完毕
     */
    void RemoveAllMessagesNotSafely();

private:

    /**
     * 是否存在消息。
     * 用于 condition_variable，在 enqueue 时置为 true，在 PollOnce 里置为 false。
     * 避免先 notify 再 wait 时，永久等待的问题
     */
    bool has_message_;

    Message *header_;

    /**
     * 保证头结点线程安全的锁
     */
    std::recursive_mutex header_mutex_;

    std::condition_variable condition_;

    std::mutex poll_once_mutex_;

    /**
     * 是否退出消息循环
     */
    bool is_quited_;

    /**
     * 用于判断是否退出的锁，保证 isQuited 线程安全
     */
    std::mutex quit_mutex_;

    std::string name_;
};


#endif //THREAD_MESSAGE_QUEUE_H
