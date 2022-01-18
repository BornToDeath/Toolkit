//
// Created by lixiaoqing on 2021/8/8.
//

#ifndef LOG_LOGCOMMON_H
#define LOG_LOGCOMMON_H


/* ======================================================= */
/*                    Log 相关常量定义                       */
/* ======================================================= */


/********** 需手动配置的常量 ***********/


/**
 * 是否是 debug 环境
 */
#ifdef BUILD_TYPE_DEBUG
#define DEBUG 1
#else
#define DEBUG 0
#endif

/**
 * mmap缓存映射长度（也即日志文件大小）。单位：Byte
 * 注意：需要设置为内存页大小（4KB）的整数倍
 */
#ifndef LOG_MMAP_LEN
#define LOG_MMAP_LEN (2 * 1024 * 1024) // 2MB
#endif

/**
 * 历史日志文件所能占用的总的磁盘大小。单位：Byte
 * 考虑到 current.txt 也占用 LOG_MMAP_LEN MB ，历史日志只能占用 (LOG_FILE_TOTAL_DISK_SIZE - LOG_MMAP_LEN) MB 。
 */
#ifndef LOG_FILE_TOTAL_DISK_SIZE
#define LOG_FILE_TOTAL_DISK_SIZE (58 * 1024 * 1024)  // 58MB
#endif

/**
 * 内存缓存大小。单位：Byte
 */
#ifndef LOG_MEMORY_LEN
#define LOG_MEMORY_LEN (4 * 1024) // 4KB
#endif


/********** 无需配置（可默认）的常量 ***********/


/**
 * 获取日志级别的名称。与 LogLevel 对应
 */
const char *const logLevelName[4] = {
        "D",  /* Debug */
        "I",  /* Info */
        "W",  /* Warn */
        "E"   /* Error */
};

/**
 * 日志缓存策略
 */
enum class LogStrategy : int {
    LOG_MEMORY_FAIL = -2, /* 标识内存缓存启用失败 */
    LOG_MMAP_FAIL = -1, /* 标识 mmap 缓存启用失败 */
    LOG_MMAP = 1,  /* 标识 mmap 缓存启用成功 */
    LOG_MEMORY = 2   /* 标识内存缓存启用成功 */
};

/**
 * 单条日志最大长度。单位：Byte
 */
#ifndef LOG_TEXT_MAX_LEN
#define LOG_TEXT_MAX_LEN  (1024 * 2)  // 2KB
#endif

/**
 * 日志文件名
 */
#ifndef LOG_CUR_FILE_NAME
#define LOG_CUR_FILE_NAME "current"
#endif

/**
 * 日志文件名的前缀。注意：此前缀不可随意更改！
 * 日志文件名示例：log_123_20210127123050.txt
 */
#ifndef LOG_FILE_PRE
#define LOG_FILE_PRE "log_"
#endif

/**
 * 日志文件名的后缀
 */
#ifndef LOG_FILE_SUF
#define LOG_FILE_SUF ".txt"
#endif

/**
 * 日志压缩文件的后缀
 */
#ifndef LOG_ZIP_FILE_SUF
#define LOG_ZIP_FILE_SUF ".zip"
#endif

/**
 * 历史日志文件个数的最大值
 */
#ifndef LOG_FILE_MAX_NUM
#define LOG_FILE_MAX_NUM static_cast<int>(LOG_FILE_TOTAL_DISK_SIZE / LOG_MMAP_LEN)
#endif

/**
 * 日志压缩文件的大小阈值。超出此值则不上传
 */
#ifndef LOG_ZIP_FILE_MAX_SIZE
#define LOG_ZIP_FILE_MAX_SIZE (20 * 1024 * 1024)  //20MB
#endif

/**
 * 日志文件状态
 */
enum class LogFileStatus : int {
    FAILED = -2,  /* 标识日志文件打开失败 */
    FULL = -1,  /* 标识日志文件已满 */
    OK = 0,   /* 标识日志文件可用 */
};

/**
 * 日志存储线程名
 */
#ifndef LOG_THREAD_NAME
#define LOG_THREAD_NAME "LogThread"
#endif

/**
 * 线程名最大长度。单位：Byte
 */
#ifndef THREAD_NAME_MAX_LEN
#define THREAD_NAME_MAX_LEN 32
#endif

/**
 * 每天日志上传次数的阈值
 */
#ifndef LOG_MAX_UPLOAD_COUNT_PER_DAY
#define LOG_MAX_UPLOAD_COUNT_PER_DAY (3)
#endif

/**
 * 日志上传重试次数
 */
#ifndef LOG_UPLOAD_RETRY_COUNT
#define LOG_UPLOAD_RETRY_COUNT (3)
#endif

/**
 * 日志加密的 KEY
 */
#ifndef LOG_ENCRYPT_KEY
#define LOG_ENCRYPT_KEY "AUTONAVI_AIDOT_!@#$%^&*()_2021"
#endif

/**
 * 类型定义
 */
using BYTE = unsigned char;


#endif //LOG_LOGCOMMON_H
