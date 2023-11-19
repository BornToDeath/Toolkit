//
// Created by lixiaoqing on 2021/5/19.
//


/**
 * 系统库
 */
#include <vector>
#include <ctime>
#include <sys/mman.h>
#include <unistd.h>
#include <dirent.h>
#include <memory>
#include <cstring>


/**
 * 自定义库
 */
#include "log_mmap.h"
#include "log_common.h"
#include "log_tool.h"


/**
 * 宏定义
 */
#define TAG "LogMmap"

namespace log {

LogMmap::LogMmap(const std::string &logFileDir) :
        logMmapAddr_(nullptr),
        logMemAddr_(nullptr),
        logLengthOffset_(0),
        memBufSize_(0),
        logStrategy_(LogStrategy::LOG_MMAP_FAIL),
        logStorageHelper_(nullptr),
        logFilePath_(logFileDir + LOG_CUR_FILE_NAME LOG_FILE_SUF) {

    this->logStorageHelper_ = std::make_shared<LogStorageHelper>(logFileDir);

    // 尝试进行 Mmap 映射，如果失败则尝试创建内存缓存
    bool isOk = this->Mmap(logFilePath_.c_str());
    if (!isOk) {
        if (DEBUG) {
            tool::PrintLog(LogLevel::Error, TAG, LOG_THREAD_NAME,
                           ">>> Mmap 映射失败！映射的文件为：%s", logFilePath_.c_str());
        }
//        Log::setIsInit(false);  // 无需调用此方法，因为如果设置为 false 则影响其他类型日志的存储
    }
}

LogMmap::~LogMmap() {

    // 取消映射
    Munmap();

    if (this->logMmapAddr_) {
        delete[] this->logMmapAddr_;
        this->logMmapAddr_ = nullptr;
    }
    if (this->logMemAddr_) {
        delete[] this->logMemAddr_;
        this->logMemAddr_ = nullptr;
    }
}

/**
 * 创建 MMAP 映射，如果 MMAP 失败则尝试创建内存 buffer
 * 流程：
 *   1. 首先打开文件，判断文件大小是否符合要求，不符合则用0填充
 *   2. 尝试进行 Mmap 映射
 *   3. 如果 Mmap 映射失败则尝试创建内存缓存
 *   4. 如果内存缓存失败则最终失败
 * @param filePath mmap映射的文件名（绝对路径）
 */
bool LogMmap::Mmap(const char *const filePath) {

    this->logStrategy_ = LogStrategy::LOG_MMAP_FAIL;

    // 标识日志待写入的文件位置
    long logOffset = 0;

    // 判断日志文件是否准备好
    LogFileStatus status = this->logStorageHelper_->PrepareLogFile(filePath, logOffset);

    if (status == LogFileStatus::FAILED) {

        // 日志文件打开失败
        if (DEBUG) {
            tool::PrintLog(LogLevel::Error, TAG, LOG_THREAD_NAME,
                           ">>> 打开日志文件 %s 失败，errno = %d", filePath, errno);
        }

        return false;

    } else if (status == LogFileStatus::FULL) {

        // 当前日志文件恰好已满
        if (DEBUG) {
            tool::PrintLog(LogLevel::Debug, TAG, LOG_THREAD_NAME,
                           ">>> 当前日志文件已满，创建新的日志文件");
        }

        /**
         * 先删除历史日志文件，再创建新的日志文件
         */

        // 删除历史日志文件
        int deleteFileNums = this->logStorageHelper_->DeleteOldestLogFile();
        if (DEBUG) {
            tool::PrintLog(LogLevel::Info, TAG, LOG_THREAD_NAME,
                           ">>> 删除历史日志文件 %d 个", deleteFileNums);
        }

        // 创建新的日志文件并重新映射
        this->logLengthOffset_ = 0;

        bool isOk = this->logStorageHelper_->RenameLogFile();
        if (!isOk) {
            return false;
        }

        // 进行 Mmap 映射
        isOk = this->Mmap(filePath);

        return isOk;

    } else if (status != LogFileStatus::OK) {

        if (DEBUG) {
            tool::PrintLog(LogLevel::Error, TAG, LOG_THREAD_NAME,
                           ">>> 日志文件不可用，Mmap 映射失败！status=%d",
                           static_cast<std::underlying_type<LogFileStatus>::type>(status));
        }
        return false;
    }

    // 当前日志文件未满，更新日志文件中日志数据的索引值
    this->logLengthOffset_ = logOffset;

    /**
     * 下面进行 Mmap 映射
     */

    /**
     * open()   打开文件，返回一个文件描述符
     * flags:   O_CREAT 若文件不存在则创建它。使用此选项时需要提供第三个参数modes,表示该文件的访问权限。
     *          O_EXCL 如果同时指定了O_CREAT,并且文件已存在，则出错返回-1。此时，errno = EEXIST
     *          O_RDWR 可读可写打开
     * return:  打开会将失败原因写入全局变量errno中。
     *          关于errno各个取值含义参考：
     *          https://blog.csdn.net/kofiory/article/details/5790409
     */
    int fd = open(filePath, O_CREAT | O_RDWR, 0660);

    if (-1 == fd) {
        if (DEBUG) {
            tool::PrintLog(LogLevel::Error, TAG, LOG_THREAD_NAME,
                           ">>> 打开日志文件 %s 失败: errno = %d", filePath, errno);
        }
        return false;
    }

    /**
     * 进行mmap映射。
     * 函数原型：void* mmap(void* __addr, size_t __size, int __prot, int __flags, int __fd, off_t __offset)
     * __addr  :       指向欲映射的内存起始地址，通常设置为NULL，代表让系统自动选定地址
     * __size  :       映射区的长度，代表将文件中多大的部分映射到内存
     * __prot  :       映射区域的保护方式，即文件打开方式。
     *     PROT_READ   映射区域可被读取
     *     PROT_WRITE  映射区域可被写入
     * __flags :       影响映射区域的各种特性
     *     MAP_SHARED  对映射区域的写入数据会复制回文件内，而且允许其他映射该文件的进程共享
     * __fd    :       要映射到内存中的文件描述符
     * __offset:       文件映射的偏移量。通常为0，代表从文件首开始映射。最好是内存页大小的整数倍
     * @return         映射成功：返回映射的内存起始地址
     *                 映射失败：返回 MAP_FAILED
     * 参考     :       https://juejin.im/post/6844903855235268615
     */
    char *pMap = (char *) ::mmap(nullptr, LOG_MMAP_LEN, PROT_READ | PROT_WRITE,
                                 MAP_SHARED, fd, 0);

    if (nullptr != pMap && pMap != MAP_FAILED) {

        // 映射成功，走mmap缓存。
        this->logStrategy_ = LogStrategy::LOG_MMAP;

        // 记录映射的起始内存地址
        this->logMmapAddr_ = pMap;

    } else {
        // 映射失败，创建内存缓存，如果内存缓存创建失败，则代表失败

        if (DEBUG) {
            tool::PrintLog(LogLevel::Warn, TAG, LOG_THREAD_NAME,
                           ">>> mmap映射失败！尝试创建内存缓存！");
        }

        char *memAddr = (char *) malloc(LOG_MEMORY_LEN);

        if (nullptr != memAddr) {

            this->logStrategy_ = LogStrategy::LOG_MEMORY;

            memset(memAddr, 0, LOG_MEMORY_LEN);

            // 记录内存缓存的起始地址
            this->logMemAddr_ = memAddr;
            this->memBufSize_ = 0;

        } else {

            this->logStrategy_ = LogStrategy::LOG_MEMORY_FAIL;

            if (DEBUG) {
                tool::PrintLog(LogLevel::Error, TAG, LOG_THREAD_NAME,
                               ">>> 内存缓存创建失败！！！");
            }
        }
    }

    /**
     * 关闭文件描述符
     * 不影响映射，映射依然存在，因为映射的是磁盘的地址，不是文件本身，和文件描述符无关。
     */
    close(fd);

    if (DEBUG) {
        tool::PrintLog(LogLevel::Debug, TAG, LOG_THREAD_NAME,
                       ">>> 日志存储策略：%s",
                       tool::GetLogStrategyName(this->logStrategy_).c_str());

        if (this->logStrategy_ == LogStrategy::LOG_MMAP) {

            tool::PrintLog(LogLevel::Debug, TAG, LOG_THREAD_NAME,
                           ">>> MMap映射长度：%ld 字节", LOG_MMAP_LEN);

        } else if (this->logStrategy_ == LogStrategy::LOG_MEMORY) {

            tool::PrintLog(LogLevel::Debug, TAG, LOG_THREAD_NAME,
                           ">>> 内存缓存大小：%ld 字节", LOG_MEMORY_LEN);

        }
    }
    return true;
}


void LogMmap::Mmap(size_t length, int prot, int flags, int fd, off_t offset) {

    this->logMmapAddr_ = (char *) ::mmap(nullptr, length, prot, flags, fd, offset);

    if (this->logMmapAddr_ == MAP_FAILED) {
        this->logMmapAddr_ = nullptr;
        return;
    }
}


void LogMmap::Munmap() {

    if (this->logStrategy_ != LogStrategy::LOG_MMAP) {
        if (DEBUG) {
            tool::PrintLog(LogLevel::Warn, TAG, LOG_THREAD_NAME,
                           ">>> 不是 Mmap 缓存策略，不能调用 Munmap() 方法！");
        }
        return;
    }

    if (nullptr == this->logMmapAddr_) {
        return;
    }

    /**
     * 取消映射，解除映射关系。
     * 原型：int Munmap(void* __addr, size_t __size);
     * __addr   映射区的起始地址
     * __size   映射长度
     * @return  成功返回 0
     *          失败返回 -1
     */
    int ret = ::munmap(this->logMmapAddr_, LOG_MMAP_LEN);
    if (ret == -1) {
        if (DEBUG) {
            tool::PrintLog(LogLevel::Warn, TAG, LOG_THREAD_NAME,
                           ">>> 取消映射失败！errno = %d", errno);
        }
        return;
    }

    // 重置相关参数

    this->logMmapAddr_ = nullptr;
    this->logMemAddr_ = nullptr;

    this->logLengthOffset_ = 0;
    this->memBufSize_ = 0;

    this->logStrategy_ = LogStrategy::LOG_MMAP_FAIL;

}

/**
 * 保存日志数据
 * @param logData
 */
void LogMmap::Msync(bool bSync) {

    if (this->logStrategy_ != LogStrategy::LOG_MMAP) {
        if (DEBUG) {
            tool::PrintLog(LogLevel::Debug, TAG, LOG_THREAD_NAME,
                           ">>> 不是 Mmap 缓存策略，不能同步！当前缓存策略是：%s",
                           tool::GetLogStrategyName(this->logStrategy_).c_str());
        }
        return;
    }

    int ret = 0;

    if (bSync) {
        // MS_SYNC: 同步写回，等待写操作完成后才返回
        ret = ::msync(this->logMmapAddr_, LOG_MMAP_LEN, MS_SYNC | MS_INVALIDATE);
    } else {
        // MS_ASYNC: 异步写回，只是将写操作排队，并不等待写操作完成就返回
        ret = ::msync(this->logMmapAddr_, LOG_MMAP_LEN, MS_ASYNC | MS_INVALIDATE);
    }

    if (ret != 0) {
        return;
    }

}

bool LogMmap::WriteLogToFileInMmap(const std::shared_ptr<LogData> &logData, bool bSync) {

    // 将日志写入mmap缓存
    bool isOk = this->FlushLogToMmap(logData, bSync);
    if (isOk) {
        return true;
    }

    /**
     * 日志长度超过mmap映射长度，说明当前日志文件已满，需要创建新的日志文件并重新映射
     */

    if (DEBUG) {
        tool::PrintLog(LogLevel::Info, TAG, LOG_THREAD_NAME,
                       ">>> (MMAP)当前日志文件 %s 已满，需要创建新的日志文件",
                       this->logFilePath_.c_str());
    }

    // 解除映射关系，会自动将mmap缓存中的日志数据回写到磁盘
    this->Munmap();

    /**
     * 以下逻辑：
     * 1. 先删除历史日志文件
     * 2. 将当前日志文件重命名
     * 3. 新建日志文件，并重新映射
     * 4. 将当前日志写入新日志文件
     */

    // 【1】删除历史日志文件
    int deleteFileNums = this->logStorageHelper_->DeleteOldestLogFile();

    if (DEBUG) {
        tool::PrintLog(LogLevel::Info, TAG, LOG_THREAD_NAME,
                       ">>> 删除历史日志文件 %d 个", deleteFileNums);
    }

    // 【2】将当前日志文件重命名
    this->logStorageHelper_->RenameLogFile();
    this->logLengthOffset_ = 0;

    // 【3】新建日志文件，并重新映射
    isOk = this->Mmap(this->logFilePath_.c_str());
    if (!isOk) {
        if (DEBUG) {
            tool::PrintLog(LogLevel::Error, TAG, LOG_THREAD_NAME, ">>> Mmap 映射失败！");
        }
        return false;
    }

    // 【4】将当前条日志写入新日志文件
    if (this->logStrategy_ == LogStrategy::LOG_MMAP) {
        this->FlushLogToMmap(logData, bSync);
    } else if (this->logStrategy_ == LogStrategy::LOG_MEMORY) {
        this->FlushLogToMem(logData);
    } else {
        return false;
    }

    return true;
}

bool LogMmap::WriteLogToFileInMem(const std::shared_ptr<LogData> &logData) {

    // 考虑到日志文件的大小是1M的原则，即使是内存缓存，也要判断是否超过1M
    if (this->logLengthOffset_ + logData->GetLog().size() <= LOG_MMAP_LEN) {

        if (this->FlushLogToMem(logData)) {
            return true;
        }

        if (DEBUG) {
            tool::PrintLog(LogLevel::Debug, TAG, LOG_THREAD_NAME,
                           ">>> 内存缓存满，将内存缓存中的数据写入文件");
        }

        // 注意，打开文件的模式必须是 "rb+"，因为 "wb+" 会清空文件，"ab+" 则是向文件追加（fseek() 也无效）
        FILE *fp = fopen(this->logFilePath_.c_str(), "rb+");

        if (fp == nullptr) {
            if (DEBUG) {
                tool::PrintLog(LogLevel::Debug, TAG, LOG_THREAD_NAME,
                               ">>> 打开日志文件 %s 失败（内存缓存）！errno = %d",
                               this->logFilePath_.c_str(), errno);
            }
            return false;
        }

        // 内存缓存满，将缓存数据写入文件
        fseek(fp, this->logLengthOffset_, SEEK_SET);

        int isOk = fprintf(fp, "%s", this->logMemAddr_);

        if (isOk == EOF) {
            if (DEBUG) {
                tool::PrintLog(LogLevel::Debug, TAG, LOG_THREAD_NAME,
                               ">>> 写入日志数据到文件 %s 失败（内存缓存）！", this->logFilePath_.c_str());
            }
            fclose(fp);
            return false;
        }

        // 将用户进程缓冲区中的数据刷新到内核缓冲区
        fflush(fp);

        // 将内核缓冲区中的数据刷新到磁盘
        fsync(fileno(fp));

        // 关闭文件流
        fclose(fp);

        // 更新有效日志的长度
        this->logLengthOffset_ += this->memBufSize_;

        // 清空内存缓存
        memset(this->logMemAddr_, 0, LOG_MEMORY_LEN);
        this->memBufSize_ = 0;

        return true;

    }

    if (DEBUG) {
        tool::PrintLog(LogLevel::Info, TAG, LOG_THREAD_NAME,
                       ">>> (MEM)当前日志文件 %s 已满，需要创建新的日志文件",
                       this->logFilePath_.c_str());
    }

    // 删除历史日志文件
    int deleteFileNums = this->logStorageHelper_->DeleteOldestLogFile();

    if (DEBUG) {
        tool::PrintLog(LogLevel::Info, TAG, LOG_THREAD_NAME,
                       ">>> 删除历史日志文件 %d 个", deleteFileNums);
    }

    // 重命名当前日志文件
    this->logStorageHelper_->RenameLogFile();

    // 标识日志待写入的文件位置
    long logOffset = 0;

    // 创建新日志文件，并写入当前日志
    LogFileStatus status = this->logStorageHelper_->PrepareLogFile(this->logFilePath_.c_str(), logOffset);

    if (status == LogFileStatus::FAILED) {
        // 日志文件打开失败
        if (DEBUG) {
            tool::PrintLog(LogLevel::Error, TAG, LOG_THREAD_NAME,
                           ">>> 打开日志文件 %s 失败", this->logFilePath_.c_str());
        }
        return false;
    }

    this->logLengthOffset_ = logOffset;

    // 清空内存缓存
    memset(this->logMemAddr_, 0, LOG_MEMORY_LEN);

    // 重置相关参数
    this->memBufSize_ = 0;

    // 将当前日志数据拷贝至内存缓存中
    this->FlushLogToMem(logData);

    return true;
}

bool LogMmap::FlushLogToMmap(const std::shared_ptr<LogData> &logData, bool bSync) {

    // 判断是否超出 Mmap 映射长度
    if (this->logLengthOffset_ + logData->GetLog().size() > LOG_MMAP_LEN) {
        return false;
    }

    // 写入mmap缓存，一段时间后会自动写回磁盘，如果想立即写回磁盘需要调用 msync()
    memcpy(this->logMmapAddr_ + this->logLengthOffset_, logData->GetLog().c_str(),
           logData->GetLog().size());

    this->logLengthOffset_ += static_cast<long>(logData->GetLog().size());

    // 是否立即同步数据到磁盘，一般无需立即同步
    if (bSync) {
        this->Msync(false);
    }

    return true;
}

bool LogMmap::FlushLogToMem(const std::shared_ptr<LogData> &logData) {

    // 判断是否超出内存缓存长度
    if (this->memBufSize_ + logData->GetLog().size() > LOG_MEMORY_LEN) {
        return false;
    }

    // 保存到内存中
    memcpy(this->logMemAddr_ + this->memBufSize_, logData->GetLog().c_str(),
           logData->GetLog().size());

    this->memBufSize_ += static_cast<long>(logData->GetLog().size());
    return true;
}

LogStrategy LogMmap::GetLogStrategy() const {
    return this->logStrategy_;
}

}  // namespace log
