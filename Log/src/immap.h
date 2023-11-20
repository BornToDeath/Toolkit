//
// Created by lixiaoqing on 2021/5/19.
//

#ifndef LOG_IMAP_H
#define LOG_IMAP_H

/**
 * 系统头文件
 */
#include <fcntl.h>

/**
 * 自定义头文件
 */
#include "log_data.h"

namespace log {

/**
 * mmap基类。提供mmap相关接口
 */
class IMmap {

/* ======================================================= */
protected: /* Methods                                      */
/* ======================================================= */

    /**
     * 只能本类和子类构造
     */
    IMmap() = default;

/* ======================================================= */
public: /* Methods                                         */
/* ======================================================= */

    /**
     * 虚析构
     */
    virtual ~IMmap() = default;

    /**
     * 禁用拷贝构造
     * @param mmap
     */
    IMmap(const IMmap &mmap) = delete;

    /**
     * 禁用赋值构造
     * @param mmap
     * @return
     */
    IMmap &operator=(const IMmap &mmap) = delete;

public:

    /**
     * mmap映射
     * @param file 待映射的文件名
     */
    virtual bool Mmap(const char *file) = 0;

    /**
     * 映射到进程空间
     * @param length 映射到进程地址空间的字节数,从offset个字节算起
     * @param prot 指定共享内存的访问权限,可取值为(或组合):
     *              PROT_READ: 可读
     *              PROT_WRITE: 可写
     *              PROT_EXEC : 可执行
     *              PROT_NONE: 不可访问
     * @param flags 可取的值为：MAP_SHARED,MAP_PRIVATE,MAP_FIXED，一般取:MAP_SHARED
     * @param fd 进程空间中的文件描述符
     * @param offset 文件的偏移地址, 一般为0
     */
    virtual void Mmap(size_t length, int prot, int flags, int fd, off_t offset) = 0;

    /**
     * 取消mmap映射
     */
    virtual void Munmap() = 0;

    /**
     * 同步内容
     * @param sync true : 同步写回
     *             false: 异步写回
     */
    virtual void Msync(bool sync) = 0;
};

}  // namespace log

#endif //LOG_IMAP_H
