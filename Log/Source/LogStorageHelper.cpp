//
// Created by lixiaoqing on 2021/5/19.
//



/**
 * 系统头文件
 */
#include <cstdio>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sstream>
#include <cstring>
#include <climits>
#include <utility>
#include <iomanip>

/**
 * 自定义头文件
 */
#include "LogStorageHelper.h"
#include "LogTools.h"
#include "LogCommon.h"


/**
 * 自定义宏
 */
#define TAG "LogStorageHelper"


LogStorageHelper::LogStorageHelper(std::string  logFileDir) :
        logFileDir(std::move(logFileDir)),
        logFileMinIndex(LONG_MAX),
        logFileMaxIndex(-1),
        logFileNums(0) {

}

LogFileStatus LogStorageHelper::prepareLogFile(const char *const filePath, long &logOffset) {

    if (filePath == nullptr || strlen(filePath) == 0) {
        LogTools::printLog(LogLevel::Error, TAG, LOG_THREAD_NAME, ">>> 日志文件为空！");
        return LogFileStatus::FAILED;
    }

    /**
     * 由于mmap需要映射固定大小的文件。所以下面的逻辑是：
     * 判断文件大小是否符合指定大小，如果不符合，填充至固定大小
     */

    // 注意，打开文件的模式必须是 "rb+"，因为 "wb+" 会清空文件，"ab+" 则是
    // 向文件追加（fseek() 也无效）。但 "rb+" 必须打开一个已存在的文件，否则出错
    FILE *file = fopen(filePath, "rb+");

    if (nullptr == file) {

        // 如果文件不存在，创建文件
        if (errno == 2) {
            file = fopen(filePath, "wb+");
            if (file == nullptr) {
                LogTools::printLog(LogLevel::Error, TAG, LOG_THREAD_NAME,
                                   ">>> 创建日志文件 %s 失败，errno = %d", filePath, errno);
                return LogFileStatus::FAILED;
            }
            LogTools::printLog(LogLevel::Debug, TAG, LOG_THREAD_NAME,
                               ">>> 创建新的日志文件：%s", filePath);
        } else {
            if (DEBUG) {
                LogTools::printLog(LogLevel::Error, TAG, LOG_THREAD_NAME,
                                   ">>> 打开日志文件 %s 失败，errno = %d", filePath, errno);
            }
            return LogFileStatus::FAILED;
        }
    }

    /**
     * 将读写位置指向文件尾后再增加offset个位移量
     * fseek (FILE* stream, long offset, int whence)
     * 作用：移动文件流指针。
     * offset: 偏移量
     * whence: SEEK_END 文件尾
     *         SEEK_SET 文件头
     */
    fseek(file, 0, SEEK_END);

    // 得到文件指针的当前位置相对于文件首的偏移量。此处是获取文件的大小
    long fileSize = ftell(file);

    if (DEBUG) {
        LogTools::printLog(LogLevel::Debug, TAG, LOG_THREAD_NAME,
                           ">>> 日志文件 %s 的大小为：%ld 字节", filePath, fileSize);
    }

    /**
     * 下面的逻辑是：判断文件大小是否满足指定大小，小于则用0补齐.
     * 目的是：防止磁盘上的脏数据影响到文件内容
     */

    if (fileSize == 0) {
        // 说明是新创建的日志文件。不管是mmap还是内存缓存，都会走此流程，意味着：日志文件在创建时都会被填充0

        // 更新日志长度
        logOffset = 0;

        // 移动文件指针到文件头
        fseek(file, 0, SEEK_SET);

        unsigned long long t1 = LogTools::currentTimeMills();

        /**
         * 分多次写 0，原因是：尽可能少的申请栈空间，避免出现问题
         */

        constexpr int flushTimes = 4;
        constexpr int flushSizePerTime = LOG_MMAP_LEN / flushTimes;

        // 不要在栈上申请空间，否则可能会导致栈溢出！
        char *zeroData = new char[flushSizePerTime];
        memset(zeroData, 0, flushSizePerTime);

        for (int i = 0; i < flushTimes; ++i) {
            // fwrite()之后会自动偏移文件指针
            fwrite(zeroData, flushSizePerTime, 1, file);
        }

        delete[] zeroData;

        // 将用户进程缓冲区中的数据刷新到内核缓冲区
        fflush(file);

        // 将内核缓冲区中的数据立即刷新到磁盘
        fsync(fileno(file));

        unsigned long long t2 = LogTools::currentTimeMills();

        // 经测试，向文件中写1M大小的 0 大概需要2毫秒
        if (DEBUG) {
            LogTools::printLog(LogLevel::Debug, TAG, LOG_THREAD_NAME,
                               ">>> 向文件写长度为 %ld 的 0 耗时：%llu 毫秒",
                               LOG_MMAP_LEN, t2 - t1);
        }

    } else if (fileSize < LOG_MMAP_LEN) {
        // 说明日志文件长度不够，需要用 0 填充

        if (DEBUG) {
            LogTools::printLog(LogLevel::Debug, TAG, LOG_THREAD_NAME,
                               ">>> 已存在的未填充的日志文件：%s，文件长度为：%ld Byte，需要用 0 填充至指定长度 %ld",
                               filePath, fileSize, LOG_MMAP_LEN);
        }

        /**
         * 记录待写入的日志位置
         */

        long index = this->getLocationToBeWritten(file);

        // 更新日志长度
        logOffset = index;

        /**
         * 向文件填充 0
         */

        fseek(file, 1, SEEK_END);

        // 需要填充的长度
        long needFilledLen = static_cast<long>(LOG_MMAP_LEN) - fileSize;

        // 不要在栈上申请空间，否则可能会导致栈溢出！
        char *buf = new char[needFilledLen];
        memset(buf, 0, needFilledLen);

        fwrite(buf, needFilledLen, 1, file);
        fflush(file);
        fsync(fileno(file));

        delete[] buf;

    } else {

        if (DEBUG) {
            LogTools::printLog(LogLevel::Debug, TAG, LOG_THREAD_NAME,
                               ">>> 已存在的已填充的日志文件：%s，文件长度为：%ld Byte",
                               filePath, fileSize);
        }

        // 获取待写入的文件位置
        long index = this->getLocationToBeWritten(file);

        if (index >= LOG_MMAP_LEN) {
            // 说明当前日志文件恰好已满
            if (DEBUG) {
                LogTools::printLog(LogLevel::Debug, TAG, LOG_THREAD_NAME,
                                   ">>> 当前日志文件已满：%s", filePath);
            }
            return LogFileStatus::FULL;
        }

        // 更新日志长度
        logOffset = index;
    }

    fclose(file);

    return LogFileStatus::OK;
}

bool LogStorageHelper::renameLogFile() {

    std::string curLogFileName = this->logFileDir + LOG_CUR_FILE_NAME LOG_FILE_SUF;

    this->updateLogFileParams();

    // 文件索引。设置字段宽度为3，不足补0
    long curFileMaxIndex = this->logFileMaxIndex + 1;
    std::ostringstream indexOss;
    indexOss << std::setw(3) << std::setfill('0') << curFileMaxIndex;

    // 当前时间戳
    std::string nowDate = LogTools::getCurrentDateTime("%Y%m%d%H%M%S");

    // 重命名后的日志文件名。示例：log_012_20210127123050.254.txt
    std::ostringstream oss;
    oss << this->logFileDir
    << LOG_FILE_PRE
    << indexOss.str()
    << "_"
    << nowDate
    << LOG_FILE_SUF;

    std::string newLogFileName = oss.str();

    if (-1 == rename(curLogFileName.c_str(), newLogFileName.c_str())) {
        if (DEBUG) {
            LogTools::printLog(LogLevel::Error, TAG, LOG_THREAD_NAME,
                               ">>> 日志文件重命名为 %s 失败！errno = %d",
                               newLogFileName.c_str(), errno);
        }
        return false;
    }

    if (DEBUG) {
        LogTools::printLog(LogLevel::Debug, TAG, LOG_THREAD_NAME,
                           ">>> 日志文件重命名为 %s 成功", newLogFileName.c_str());
    }

    return true;
}

bool LogStorageHelper::updateLogFileParams() {

    /**
     * 更新日志文件相关参数，包括：
     * 1. 日志文件名最大索引
     * 2. 日志文件名最小索引
     * 3. 日志文件的个数
     */

    // 日志文件名的最大索引
    long tLogFileMaxIndex = -1;

    // 日志文件名的最小索引
    long tLogFileMinIndex = LONG_MAX;

    // 日志文件的个数
    long tLogFileNums = 0;

    DIR *dir;
    if ((dir = opendir(this->logFileDir.c_str())) == nullptr) {
        if (DEBUG) {
            LogTools::printLog(LogLevel::Error, TAG, LOG_THREAD_NAME,
                               ">>> 打开日志所在目录 %s 失败，errno = %d", this->logFileDir.c_str(), errno);
        }
        return false;
    }

    struct dirent *ent;

    // 获取目录下的每个文件的文件名。注意：readdir()是非线程安全函数
    while ((ent = readdir(dir)) != nullptr) {

        char *filename = ent->d_name;

        // 字符串比较判断是否是日志文件
        if (0 != strncmp(filename, LOG_FILE_PRE, strlen(LOG_FILE_PRE))) {
            continue;
        }

        ++tLogFileNums;

        long tempIndex = 0;

        // 获取当前日志文件的索引值
        for (int i = static_cast<int>(strlen(LOG_FILE_PRE)); i < strlen(filename); ++i) {
            if (filename[i] >= '0' && filename[i] <= '9') {
                tempIndex = tempIndex * 10 + (filename[i] - '0');
            } else {
                break;
            }
        }

        if (tempIndex > tLogFileMaxIndex) {
            tLogFileMaxIndex = tempIndex;
        }

        if (tempIndex < tLogFileMinIndex) {
            tLogFileMinIndex = tempIndex;
        }

    }

    closedir(dir);

    if (tLogFileNums == 0) {
        // 说明日志目录下尚没有日志文件
        tLogFileMaxIndex = 0;
        tLogFileMinIndex = 0;
    }

    this->logFileMaxIndex = tLogFileMaxIndex;
    this->logFileMinIndex = tLogFileMinIndex;
    this->logFileNums = tLogFileNums;

    return true;
}

int LogStorageHelper::deleteOldestLogFile() {

    this->updateLogFileParams();

    if (this->logFileNums < LOG_FILE_MAX_NUM) {
        return 0;
    }

    /**
     * 历史日志文件如超出磁盘空间的限制，则删除最老的日志文件
     */

    int deleteFileNums = 0;

    // 待删除的历史日志文件的索引最大值：[-oo, end]
    long end = this->logFileMinIndex + (this->logFileNums - LOG_FILE_MAX_NUM);

    DIR *dir;
    if ((dir = opendir(this->logFileDir.c_str())) == nullptr) {
        if (DEBUG) {
            LogTools::printLog(LogLevel::Error, TAG, LOG_THREAD_NAME,
                               ">>> 打开日志所在目录 %s 失败，errno = %d", this->logFileDir.c_str(), errno);
        }
        return false;
    }

    struct dirent *ent;

    // 获取目录下的每个文件的文件名。注意：readdir()是非线程安全函数
    while ((ent = readdir(dir)) != nullptr) {

        char *filename = ent->d_name;

        // 字符串比较判断是否是日志文件
        if (0 != strncmp(filename, LOG_FILE_PRE, strlen(LOG_FILE_PRE))) {
            continue;
        }

        long tempIndex = 0;

        // 获取当前日志文件的索引值
        for (int i = static_cast<int>(strlen(LOG_FILE_PRE)); i < strlen(filename); ++i) {
            if (filename[i] < '0' || filename[i] > '9') {
                break;
            }
            tempIndex = tempIndex * 10 + (filename[i] - '0');
        }

        if (tempIndex > end) {
            continue;
        }

        std::string filePath = this->logFileDir + filename;

        // 删除日志文件
        int isOk = ::remove(filePath.c_str());
        if (isOk == EOF) {
            if (DEBUG) {
                LogTools::printLog(LogLevel::Error, TAG, LOG_THREAD_NAME,
                                   ">>> 删除历史日志文件 %s 失败，errno = %d", filePath.c_str(), errno);
            }
            continue;
        }

        if (DEBUG) {
            LogTools::printLog(LogLevel::Info, TAG, LOG_THREAD_NAME,
                               ">>> 删除历史日志文件 %s 成功", filePath.c_str());
        }

        ++deleteFileNums;
    }

    closedir(dir);

    /**
     * 再次扫描日志目录，确保历史日志文件总大小不超过限制，加强保护。
     * 考虑这种情况：日志文件索引不连续
     */

    this->updateLogFileParams();

    if (this->logFileNums >= LOG_FILE_MAX_NUM) {
        deleteFileNums += this->deleteOldestLogFile();
    }

    return deleteFileNums;
}

long LogStorageHelper::getLogFileMaxIndex() const {
    return this->logFileMaxIndex;
}

long LogStorageHelper::getLogFileMinIndex() const {
    return this->logFileMinIndex;
}

long LogStorageHelper::getLogFileNums() const {
    return this->logFileNums;
}

long LogStorageHelper::getLocationToBeWritten(FILE *file) const {

    /**
     * 下面的逻辑是：查找文件中第一个 \0 的位置，此位置是之后日志要写入的位置。
     */

    fseek(file, 0, SEEK_END);

    long fileSize = ftell(file);

//    auto t1 = AIDotLog::currentTimeMills();

    long start = 0;
    long end = fileSize - 1;
    long mid = 0;
    char ch = 0;

    while (start <= end) {
        mid = start + ((end - start) >> 1);
        fseek(file, mid, SEEK_SET);
        ch = fgetc(file);
        if (ch == 0) {
            end = mid - 1;
        } else {
            start = mid + 1;
        }
    }

//    auto t2 = AIDotLog::currentTimeMills();

//    if (DEBUG) {
//        AIDotLog::printLog(LogLevel::Debug, TAG, LOG_THREAD_NAME,
//                              ">>> 二分查找耗时：%ld 毫秒", (t2 - t1));
//        AIDotLog::printLog(LogLevel::Debug, TAG, LOG_THREAD_NAME,
//                              ">>> 二分查找结束，mid = %ld, 当前字符 = %d", mid, ch);
//    }

    /**
     * 此时 mid 要么指向 0 之前的字符，要么指向 0
     */

    fseek(file, mid, SEEK_SET);

    if (fgetc(file) != 0) {
        ++mid;
    }

    return mid;
}
