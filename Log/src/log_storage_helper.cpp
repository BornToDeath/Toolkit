//
// Created by lixiaoqing on 2021/5/19.
//

#include "log_storage_helper.h"

/**
 * 系统头文件
 */
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>

#include <climits>
#include <cstdio>
#include <cstring>
#include <iomanip>
#include <sstream>
#include <utility>

/**
 * 自定义头文件
 */
#include "log_common.h"
#include "log_tool.h"

/**
 * 自定义宏
 */
#define TAG "LogStorageHelper"

namespace log {

LogStorageHelper::LogStorageHelper(std::string log_file_dir) :
        log_file_dir_(std::move(log_file_dir)),
        log_file_min_index_(LONG_MAX),
        log_file_max_index_(-1),
        log_file_nums_(0) {

}

LogFileStatus LogStorageHelper::PrepareLogFile(const char *const file_path, long &log_offset) {

    if (file_path == nullptr || strlen(file_path) == 0) {
        tool::PrintLog(LogLevel::Error, TAG, LOG_THREAD_NAME, ">>> 日志文件为空！");
        return LogFileStatus::FAILED;
    }

    /**
     * 由于mmap需要映射固定大小的文件。所以下面的逻辑是：
     * 判断文件大小是否符合指定大小，如果不符合，填充至固定大小
     */

    // 注意，打开文件的模式必须是 "rb+"，因为 "wb+" 会清空文件，"ab+" 则是
    // 向文件追加（fseek() 也无效）。但 "rb+" 必须打开一个已存在的文件，否则出错
    FILE *file = fopen(file_path, "rb+");

    if (nullptr == file) {

        // 如果文件不存在，创建文件
        if (errno == 2) {
            file = fopen(file_path, "wb+");
            if (file == nullptr) {
                tool::PrintLog(LogLevel::Error, TAG, LOG_THREAD_NAME,
                               ">>> 创建日志文件 %s 失败，errno = %d", file_path, errno);
                return LogFileStatus::FAILED;
            }
            tool::PrintLog(LogLevel::Debug, TAG, LOG_THREAD_NAME,
                           ">>> 创建新的日志文件：%s", file_path);
        } else {
            if (DEBUG) {
                tool::PrintLog(LogLevel::Error, TAG, LOG_THREAD_NAME,
                               ">>> 打开日志文件 %s 失败，errno = %d", file_path, errno);
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
    long file_size = ftell(file);

    if (DEBUG) {
        tool::PrintLog(LogLevel::Debug, TAG, LOG_THREAD_NAME,
                       ">>> 日志文件 %s 的大小为：%ld 字节", file_path, file_size);
    }

    /**
     * 下面的逻辑是：判断文件大小是否满足指定大小，小于则用0补齐.
     * 目的是：防止磁盘上的脏数据影响到文件内容
     */

    if (file_size == 0) {
        // 说明是新创建的日志文件。不管是mmap还是内存缓存，都会走此流程，意味着：日志文件在创建时都会被填充0

        // 更新日志长度
        log_offset = 0;

        // 移动文件指针到文件头
        fseek(file, 0, SEEK_SET);

        unsigned long long t1 = tool::CurrentTimeMills();

        /**
         * 分多次写 0，原因是：尽可能少的申请栈空间，避免出现问题
         */

        constexpr int kFlushTimes = 4;
        constexpr int kFlushSizePerTime = LOG_MMAP_LEN / kFlushTimes;

        // 不要在栈上申请空间，否则可能会导致栈溢出！
        char *zero_data_buf = new char[kFlushSizePerTime];
        memset(zero_data_buf, 0, kFlushSizePerTime);

        for (int i = 0; i < kFlushTimes; ++i) {
            // fwrite()之后会自动偏移文件指针
            fwrite(zero_data_buf, kFlushSizePerTime, 1, file);
        }

        delete[] zero_data_buf;

        // 将用户进程缓冲区中的数据刷新到内核缓冲区
        fflush(file);

        // 将内核缓冲区中的数据立即刷新到磁盘
        fsync(fileno(file));

        unsigned long long t2 = tool::CurrentTimeMills();

        // 经测试，向文件中写1M大小的 0 大概需要2毫秒
        if (DEBUG) {
            tool::PrintLog(LogLevel::Debug, TAG, LOG_THREAD_NAME,
                           ">>> 向文件写长度为 %ld 的 0 耗时：%llu 毫秒",
                           LOG_MMAP_LEN, t2 - t1);
        }

    } else if (file_size < LOG_MMAP_LEN) {
        // 说明日志文件长度不够，需要用 0 填充

        if (DEBUG) {
            tool::PrintLog(LogLevel::Debug, TAG, LOG_THREAD_NAME,
                           ">>> 已存在的未填充的日志文件：%s，文件长度为：%ld Byte，需要用 0 填充至指定长度 %ld",
                           file_path, file_size, LOG_MMAP_LEN);
        }

        /**
         * 记录待写入的日志位置
         */

        long index = this->GetLocationToBeWritten(file);

        // 更新日志长度
        log_offset = index;

        /**
         * 向文件填充 0
         */

        fseek(file, 1, SEEK_END);

        // 需要填充的长度
        long need_filled_len = static_cast<long>(LOG_MMAP_LEN) - file_size;

        // 不要在栈上申请空间，否则可能会导致栈溢出！
        char *buf = new char[need_filled_len];
        memset(buf, 0, need_filled_len);

        fwrite(buf, need_filled_len, 1, file);
        fflush(file);
        fsync(fileno(file));

        delete[] buf;

    } else {

        if (DEBUG) {
            tool::PrintLog(LogLevel::Debug, TAG, LOG_THREAD_NAME,
                           ">>> 已存在的已填充的日志文件：%s，文件长度为：%ld Byte",
                           file_path, file_size);
        }

        // 获取待写入的文件位置
        long index = this->GetLocationToBeWritten(file);

        if (index >= LOG_MMAP_LEN) {
            // 说明当前日志文件恰好已满
            if (DEBUG) {
                tool::PrintLog(LogLevel::Debug, TAG, LOG_THREAD_NAME,
                               ">>> 当前日志文件已满：%s", file_path);
            }
            return LogFileStatus::FULL;
        }

        // 更新日志长度
        log_offset = index;
    }

    fclose(file);

    return LogFileStatus::OK;
}

bool LogStorageHelper::RenameLogFile() {

    std::string cur_log_filename = this->log_file_dir_ + LOG_CUR_FILE_NAME LOG_FILE_SUF;

    this->UpdateLogFileParams();

    // 文件索引。设置字段宽度为3，不足补0
    long cur_file_max_index = this->log_file_max_index_ + 1;
    std::ostringstream index_oss;
    index_oss << std::setw(3) << std::setfill('0') << cur_file_max_index;

    // 当前时间戳
    std::string now_date = tool::GetCurrentDateTime("%Y%m%d%H%M%S");

    // 重命名后的日志文件名。示例：log_012_20210127123050.254.txt
    std::ostringstream oss;
    oss << this->log_file_dir_
        << LOG_FILE_PRE
        << index_oss.str()
        << "_"
        << now_date
        << LOG_FILE_SUF;

    std::string new_log_filename = oss.str();

    if (-1 == rename(cur_log_filename.c_str(), new_log_filename.c_str())) {
        if (DEBUG) {
            tool::PrintLog(LogLevel::Error, TAG, LOG_THREAD_NAME,
                           ">>> 日志文件重命名为 %s 失败！errno = %d",
                           new_log_filename.c_str(), errno);
        }
        return false;
    }

    if (DEBUG) {
        tool::PrintLog(LogLevel::Debug, TAG, LOG_THREAD_NAME,
                       ">>> 日志文件重命名为 %s 成功", new_log_filename.c_str());
    }

    return true;
}

bool LogStorageHelper::UpdateLogFileParams() {

    /**
     * 更新日志文件相关参数，包括：
     * 1. 日志文件名最大索引
     * 2. 日志文件名最小索引
     * 3. 日志文件的个数
     */

    // 日志文件名的最大索引
    long t_log_file_max_index = -1;

    // 日志文件名的最小索引
    long t_log_file_min_index = LONG_MAX;

    // 日志文件的个数
    long t_log_file_nums = 0;

    DIR *dir;
    if ((dir = opendir(this->log_file_dir_.c_str())) == nullptr) {
        if (DEBUG) {
            tool::PrintLog(LogLevel::Error, TAG, LOG_THREAD_NAME,
                           ">>> 打开日志所在目录 %s 失败，errno = %d", this->log_file_dir_.c_str(), errno);
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

        ++t_log_file_nums;

        long temp_index = 0;

        // 获取当前日志文件的索引值
        for (int i = static_cast<int>(strlen(LOG_FILE_PRE)); i < strlen(filename); ++i) {
            if (filename[i] >= '0' && filename[i] <= '9') {
                temp_index = temp_index * 10 + (filename[i] - '0');
            } else {
                break;
            }
        }

        if (temp_index > t_log_file_max_index) {
            t_log_file_max_index = temp_index;
        }

        if (temp_index < t_log_file_min_index) {
            t_log_file_min_index = temp_index;
        }

    }

    closedir(dir);

    if (t_log_file_nums == 0) {
        // 说明日志目录下尚没有日志文件
        t_log_file_max_index = 0;
        t_log_file_min_index = 0;
    }

    this->log_file_max_index_ = t_log_file_max_index;
    this->log_file_min_index_ = t_log_file_min_index;
    this->log_file_nums_ = t_log_file_nums;

    return true;
}

int LogStorageHelper::DeleteOldestLogFile() {

    this->UpdateLogFileParams();

    if (this->log_file_nums_ < LOG_FILE_MAX_NUM) {
        return 0;
    }

    /**
     * 历史日志文件如超出磁盘空间的限制，则删除最老的日志文件
     */

    int delete_file_nums = 0;

    // 待删除的历史日志文件的索引最大值：[-oo, end]
    long end = this->log_file_min_index_ + (this->log_file_nums_ - LOG_FILE_MAX_NUM);

    DIR *dir;
    if ((dir = opendir(this->log_file_dir_.c_str())) == nullptr) {
        if (DEBUG) {
            tool::PrintLog(LogLevel::Error, TAG, LOG_THREAD_NAME,
                           ">>> 打开日志所在目录 %s 失败，errno = %d", this->log_file_dir_.c_str(), errno);
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

        std::string filepath = this->log_file_dir_ + filename;

        // 删除日志文件
        int is_ok = ::remove(filepath.c_str());
        if (is_ok == EOF) {
            if (DEBUG) {
                tool::PrintLog(LogLevel::Error, TAG, LOG_THREAD_NAME,
                               ">>> 删除历史日志文件 %s 失败，errno = %d", filepath.c_str(), errno);
            }
            continue;
        }

        if (DEBUG) {
            tool::PrintLog(LogLevel::Info, TAG, LOG_THREAD_NAME,
                           ">>> 删除历史日志文件 %s 成功", filepath.c_str());
        }

        ++delete_file_nums;
    }

    closedir(dir);

    /**
     * 再次扫描日志目录，确保历史日志文件总大小不超过限制，加强保护。
     * 考虑这种情况：日志文件索引不连续
     */

    this->UpdateLogFileParams();

    if (this->log_file_nums_ >= LOG_FILE_MAX_NUM) {
        delete_file_nums += this->DeleteOldestLogFile();
    }

    return delete_file_nums;
}

long LogStorageHelper::GetLogFileMaxIndex() const {
    return this->log_file_max_index_;
}

long LogStorageHelper::GetLogFileMinIndex() const {
    return this->log_file_min_index_;
}

long LogStorageHelper::GetLogFileNums() const {
    return this->log_file_nums_;
}

long LogStorageHelper::GetLocationToBeWritten(FILE *file) const {

    /**
     * 下面的逻辑是：查找文件中第一个 \0 的位置，此位置是之后日志要写入的位置。
     */

    fseek(file, 0, SEEK_END);

    long file_size = ftell(file);

//    auto t1 = AIDotLog::CurrentTimeMills();

    long start = 0;
    long end = file_size - 1;
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

//    auto t2 = AIDotLog::CurrentTimeMills();

//    if (DEBUG) {
//        AIDotLog::PrintLog(LogLevel::Debug, TAG, LOG_THREAD_NAME,
//                              ">>> 二分查找耗时：%ld 毫秒", (t2 - t1));
//        AIDotLog::PrintLog(LogLevel::Debug, TAG, LOG_THREAD_NAME,
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

}  // namespace log