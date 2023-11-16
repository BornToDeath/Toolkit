//
// Created by lixiaoqing on 2021/5/19.
//

#ifndef LOG_LOGMMAP_H
#define LOG_LOGMMAP_H


/**
 * 系统头文件
 */
#include <iostream>

/**
 * 自定义头文件
 */
#include "IMmap.h"
#include "LogStorageHelper.h"

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
     * @param logFileDir 日志目录
     */
    explicit LogMmap(const std::string &logFileDir);

    /**
     * 虚析构函数
     */
    ~LogMmap() override;

    /**
     * 禁用拷贝构造
     * @param mmapImpl
     */
    LogMmap(const LogMmap &mmapImpl) = delete;

    /**
     * 禁用赋值构造
     * @param mmapImpl
     * @return
     */
    LogMmap &operator=(const LogMmap &mmapImpl) = delete;

public:

    /**
     * 获取日志存储策略
     * @return
     */
    LogStrategy GetLogStrategy() const;

    /**
     * 以 Mmap 策略将日志数据写入文件
     * @param logData 日志数据
     * @param bSync   是否立即同步 Mmap 缓存数据到磁盘。
     *                一般无需立即同步，Mmap 一段时间之后会自动同步
     *                true : 立即同步
     *                false: 不同步
     * @return
     */
    bool WriteLogToFileInMmap(const std::shared_ptr<LogData> &logData, bool bSync = false);

    /**
     * 以内存缓存策略将日志数据写入文件
     * @param logData
     */
    bool WriteLogToFileInMem(const std::shared_ptr<LogData> &logData);

    /**
     * 将日志刷新到 Mmap 缓存
     * @param logData 日志数据
     * @param bSync   是否立即同步mmap缓存数据到磁盘。
     *                一般无需立即同步，Mmap 一段时间之后会自动同步
     *                true : 立即同步
     *                false: 不同步
     * @return
     */
    bool FlushLogToMmap(const std::shared_ptr<LogData> &logData, bool bSync);

    /**
     * 将日志数据刷新到内存缓存
     * @param logData 日志数据
     * @return
     */
    bool FlushLogToMem(const std::shared_ptr<LogData> &logData);

private:

    /**
     * mmap映射
     * @param file 待映射的文件名
     */
    bool Mmap(const char *filePath) override;

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
     * @param bSync   true:  同步写回
     *                false: 异步写回
     */
    void Msync(bool bSync) override;

/* ======================================================= */
private: /* Fields                                         */
/* ======================================================= */

    /**
     * 日志文件名（绝对路径）
     */
    const std::string logFilePath_;

    /**
     * 如果mmap映射成功，该值表示映射到进程空间的地址
     */
    char *logMmapAddr_;

    /**
     * 如果内存缓存创建成功，该值表示内存缓存的地址
     */
    char *logMemAddr_;

    /**
     * 日志文件中有效日志数据的长度
     */
    long logLengthOffset_;

    /**
     * 如果是内存缓存映射，该值表示当前内存缓冲区大小
     */
    long memBufSize_;

    /**
     * 日志存储策略
     */
    LogStrategy logStrategy_;

    /**
     * 日志存储器，进行具体的日志文件相关操作
     */
    std::shared_ptr<LogStorageHelper> logStorageHelper_;
};

}  // namespace log

#endif //LOG_LOGMMAP_H
