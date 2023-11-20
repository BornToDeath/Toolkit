//
// Created by lixiaoqing on 2021/5/19.
//

#ifndef LOG_LOG_MMAP_H
#define LOG_LOG_MMAP_H


/**
 * 系统头文件
 */
#include <iostream>
#include <memory>

/**
 * 自定义头文件
 */
#include "immap.h"
#include "log_storage_helper.h"

namespace log {

/**
 * mmap实现类。
 * 进行mmap映射、取消映射、同步内容等操作。该类不可被继承
 */
class LogMmap final : private IMmap {

/* ======================================================= */
public: /* Methods                                         */
/* ======================================================= */

    /**
     *
     * @param log_file_dir 日志目录
     */
    explicit LogMmap(const std::string &log_file_dir);

    /**
     * 虚析构函数
     */
    ~LogMmap() override;

    /**
     * 禁用拷贝构造
     * @param mmap_impl
     */
    LogMmap(const LogMmap &mmap_impl) = delete;

    /**
     * 禁用赋值构造
     * @param mmap_impl
     * @return
     */
    LogMmap &operator=(const LogMmap &mmap_impl) = delete;

public:

    /**
     * 获取日志存储策略
     * @return
     */
    LogStrategy GetLogStrategy() const;

    /**
     * 以 Mmap 策略将日志数据写入文件
     * @param log_data 日志数据
     * @param sync   是否立即同步 Mmap 缓存数据到磁盘。
     *                一般无需立即同步，Mmap 一段时间之后会自动同步
     *                true : 立即同步
     *                false: 不同步
     * @return
     */
    bool WriteLogToFileInMmap(const std::shared_ptr<LogData> &log_data, bool sync = false);

    /**
     * 以内存缓存策略将日志数据写入文件
     * @param log_data
     */
    bool WriteLogToFileInMem(const std::shared_ptr<LogData> &log_data);

    /**
     * 将日志刷新到 Mmap 缓存
     * @param log_data 日志数据
     * @param sync   是否立即同步mmap缓存数据到磁盘。
     *                一般无需立即同步，Mmap 一段时间之后会自动同步
     *                true : 立即同步
     *                false: 不同步
     * @return
     */
    bool FlushLogToMmap(const std::shared_ptr<LogData> &log_data, bool sync);

    /**
     * 将日志数据刷新到内存缓存
     * @param log_data 日志数据
     * @return
     */
    bool FlushLogToMem(const std::shared_ptr<LogData> &log_data);

private:

    /**
     * mmap映射
     * @param file 待映射的文件名
     */
    bool Mmap(const char *file_path) override;

    /**
     * mmap映射
     * @param length
     * @param prot
     * @param flags
     * @param fd
     * @param offset
     */
    void Mmap(size_t length, int prot, int flags, int fd, off_t offset) override;

    /**
     * 解除映射关系, 解除后不能再访问这段映射空间了
     */
    void Munmap() override;

    /**
     * 将日志数据立即同步到磁盘中
     * @param logData 日志数据
     * @param sync   true :  同步写回
     *               false: 异步写回
     */
    void Msync(bool sync) override;

/* ======================================================= */
private: /* Fields                                         */
/* ======================================================= */

    /**
     * 日志文件名（绝对路径）
     */
    const std::string log_file_path_;

    /**
     * 如果mmap映射成功，该值表示映射到进程空间的地址
     */
    char *log_mmap_addr_;

    /**
     * 如果内存缓存创建成功，该值表示内存缓存的地址
     */
    char *log_mem_addr_;

    /**
     * 日志文件中有效日志数据的长度
     */
    long log_length_offset_;

    /**
     * 如果是内存缓存映射，该值表示当前内存缓冲区大小
     */
    long mem_buf_size_;

    /**
     * 日志存储策略
     */
    LogStrategy log_strategy_;

    /**
     * 日志存储器，进行具体的日志文件相关操作
     */
    std::shared_ptr<LogStorageHelper> log_storage_helper_;
};

}  // namespace log

#endif //LOG_LOG_MMAP_H
