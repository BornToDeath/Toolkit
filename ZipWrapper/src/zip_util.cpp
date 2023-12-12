#include "zip_util.h"

#include <sys/stat.h>
#include <unistd.h>

#include <cstring>

#include "unzip.h"
#include "zip.h"

#define TAG "ZipUtil"
#define log(format, args...) printf(TAG " | " format, ##args); printf("\n")

bool ZipUtil::UnzipFile(const std::string &zip_filepath, const std::string &dest_folder) {

    if (zip_filepath.empty() || dest_folder.empty()) {
        log("zip_filepath.empty() || dstFolder.empty()");
        return false;
    }

    if (!ZipUtil::IsFileExist(zip_filepath.c_str())) {
        log("zip压缩文件不存在：%s", zip_filepath.c_str());
        return false;
    }

    if (!ZipUtil::IsFolderExist(dest_folder.c_str())) {
        log("解压到的目录不存在：%s", dest_folder.c_str());
        return false;
    }

    // 1. 打开zip文件
    unzFile zip_file = unzOpen(zip_filepath.c_str());
    if (zip_file == nullptr) {
        log("unzOpen failed");
        return false;
    }

    // 2. 获取zip文件的信息
    unz_global_info global_info;
    if (unzGetGlobalInfo(zip_file, &global_info) != UNZ_OK) {
        log("unzGetGlobalInfo failed");
        unzClose(zip_file);
        return false;
    }

    // 压缩文件中的文件个数
    const int file_count = static_cast<int>(global_info.number_entry);

    // 3. 解析zip文件
    for (int i = 0; i < file_count; ++i) {
        unz_file_info64 file_info64;
        char filename[512] = {0};

        // 解析当前内部文件信息
        int is_ok = unzGetCurrentFileInfo64(zip_file, &file_info64, filename, sizeof(filename), nullptr, 0, nullptr, 0);
        if (is_ok != UNZ_OK) {
            // 只要有一个文件解析失败，就终止解压
            log("unzGetCurrentFileInfo failed");
            unzClose(zip_file);
            return false;
        }

        char name[1024] = {0};
        if (filename[strlen(filename) - 1] == '/') {
//            log("这是一个文件夹：%s", filename);
            sprintf(name, "%s/%s", dest_folder.c_str(), filename);
            if (!ZipUtil::CreateDirIfNotExist(name)) {
                log("创建文件夹失败：%s", filename);
            }
        } else {
            if (unzOpenCurrentFile(zip_file) != UNZ_OK) {
                log("unzOpenCurrentFile failed");
                unzClose(zip_file);
                return false;
            }

            sprintf(name, "%s/%s", dest_folder.c_str(), filename);
            if (WriteData(zip_file, name) < 0) {
                log("WriteData failed");
                unzCloseCurrentFile(zip_file);
                unzClose(zip_file);
                return false;
            }
        }

        unzCloseCurrentFile(zip_file);
        unzGoToNextFile(zip_file);
    }

    unzClose(zip_file);
    return true;
}

int ZipUtil::WriteData(unzFile &zip_file, const char *dest_path_name) {
    if (dest_path_name == nullptr) {
        return -1;
    }

    // 删除以.开头的文件
    std::string s_path = dest_path_name;
    int index = (int) s_path.find_last_of('/');
    std::string name = s_path.substr(index + 1);
    if (name.find('.') == 0) {
        return 0;
    }

    FILE *fp = fopen(dest_path_name, "wb");
    if (fp == nullptr) {
        return -1;
    }

    char read_buffer[1024] = {0};
    int error = UNZ_OK;

    do {
        // 读取文件
        error = unzReadCurrentFile(zip_file, read_buffer, sizeof(read_buffer));
        if (error < 0) {
            fclose(fp);
            return -1;
        }
        if (error > 0) {
            fwrite(read_buffer, error, 1, fp);
        }
    } while (error > 0);

    fclose(fp);
    return 0;
}

bool ZipUtil::UnzipFileByName(const std::string &zip_filepath, const std::string &filename,
                              const std::string &dest_folder) {
    if (zip_filepath.empty() || filename.empty()) {
        log("zip_filepath.empty() || filename.empty()");
        return false;
    }
    if (!IsFileExist(zip_filepath.c_str())) {
        log("压缩文件不存在：%s", zip_filepath.c_str());
        return false;
    }
    if (!IsFolderExist(dest_folder.c_str())) {
        log("解压到的目录不存在：%s", dest_folder.c_str());
        return false;
    }

    // 1. 打开zip文件
    unzFile file_handle = unzOpen(zip_filepath.c_str());
    if (file_handle == nullptr) {
        log("unzOpen failed");
        return false;
    }

    // 2.1 定位到指定文件
    if (unzLocateFile(file_handle, filename.c_str(), 1) != UNZ_OK) {
        log("unzLocateFile failed of the file: %s", filename.c_str());
        return false;
    }

    // 2.2 获取当前内部压缩文件的信息
    unz_file_info64 file_info64;
    char filename_buf[512] = {0};
    int is_ok = unzGetCurrentFileInfo64(file_handle, &file_info64, filename_buf, sizeof(filename_buf), nullptr, 0,
                                        nullptr, 0);
    if (is_ok != UNZ_OK) {
        log("unzGetCurrentFileInfo failed");
        unzClose(file_handle);
        return false;
    }

    // 2.3 打开当前内部压缩文件
    if (unzOpenCurrentFile(file_handle) != UNZ_OK) {
        log("unzOpenCurrentFile failed");
        unzClose(file_handle);
        return false;
    }

    // 2.4 解析当前文件到指定文件夹
    char name[1024] = {0};
    sprintf(name, "%s/%s", dest_folder.c_str(), filename_buf);
    if (WriteData(file_handle, name) < 0) {
        log("WriteData failed");
        unzCloseCurrentFile(file_handle);
        unzClose(file_handle);
        return false;
    }

    unzCloseCurrentFile(file_handle);
    unzClose(file_handle);
    return true;
}

bool ZipUtil::UnzipFileByNames(const std::string &zip_filepath, const std::vector<std::string> &filenames,
                               const std::string &dest_folder) {
    if (zip_filepath.empty() || filenames.empty()) {
        log("zip_filepath.empty() || filenames.empty()");
        return false;
    }
    if (!IsFileExist(zip_filepath.c_str())) {
        log("压缩文件不存在：%s", zip_filepath.c_str());
        return false;
    }
    if (!IsFolderExist(dest_folder.c_str())) {
        log("解压到的目录不存在：%s", dest_folder.c_str());
        return false;
    }

    // 1. 打开zip文件
    unzFile file_handle = unzOpen(zip_filepath.c_str());
    if (file_handle == nullptr) {
        log("unzOpen failed");
        return false;
    }

    // 2. 遍历文件名列表，循环解析文件
    for (const std::string &filename: filenames) {
        // 2.1 定位到指定文件
        if (unzLocateFile(file_handle, filename.c_str(), 1) != UNZ_OK) {
            log("unzLocateFile failed of the file: %s", filename.c_str());
            return false;
        }

        // 2.2 获取当前内部压缩文件的信息
        unz_file_info64 file_info64;
        char filename_buf[512] = {0};
        int is_ok = unzGetCurrentFileInfo64(file_handle, &file_info64, filename_buf, sizeof(filename_buf), nullptr, 0,
                                            nullptr, 0);
        if (is_ok != UNZ_OK) {
            // 只要有一个文件解析失败，就终止解压
            log("unzGetCurrentFileInfo failed");
            unzClose(file_handle);
            return false;
        }

        // 2.3 打开当前内部压缩文件
        if (unzOpenCurrentFile(file_handle) != UNZ_OK) {
            log("unzOpenCurrentFile failed");
            unzClose(file_handle);
            return false;
        }

        // 2.4 解析当前文件到指定文件夹
        char name[1024] = {0};
        sprintf(name, "%s/%s", dest_folder.c_str(), filename_buf);
        if (WriteData(file_handle, name) < 0) {
            log("WriteData failed");
            unzCloseCurrentFile(file_handle);
            unzClose(file_handle);
            return false;
        }

        unzCloseCurrentFile(file_handle);

        // 2.5 转到下个文件
        unzGoToNextFile(file_handle);
    }

    unzClose(file_handle);
    return true;
}

int ZipUtil::UnzipFileInMemory(std::string zip_filepath, std::map<std::string, unsigned char *> &file_map) {

    int return_value;
    //打开zip文件
    unzFile unz_file = unzOpen(zip_filepath.c_str());
    if (unz_file == NULL) {
        return -1;
    }
    //获取zip文件 记得释放
    unz_global_info *global_info = new unz_global_info;
    return_value = unzGetGlobalInfo(unz_file, global_info);
    if (return_value != UNZ_OK) {
        return -1;
    }
    //解析zip文件
    unz_file_info ufi;
    unz_file_info *file_info = &ufi;
    const int MAX_PATH = 256;
    //存放从zip中解析出来的内部文件
    char sz_zip_filename[MAX_PATH];
    for (int i = 0; i < global_info->number_entry; i++) {
        //解析得到zip中的文件信息
        return_value = unzGetCurrentFileInfo(unz_file, file_info, sz_zip_filename, MAX_PATH, NULL, 0, NULL, 0);
        if (return_value != UNZ_OK) {
            return -1;
        }
        // 当做文件夹处理
        if (file_info->uncompressed_size == 0) {
            // 数据集中没有文件夹 都是文件
            continue;
        }
        //打开文件
        return_value = unzOpenCurrentFile(unz_file);
        if (return_value != UNZ_OK) {
            return -3;
        }
        unsigned long length = file_info->uncompressed_size;
        // 记得内存释放
        unsigned char *data = new unsigned char[length];
        //读取文件
        const int BUFFER_SIZE = 4096;
        char sz_read_buffer[BUFFER_SIZE];
        while (true) {
            memset(sz_read_buffer, 0, BUFFER_SIZE);
            int read_file_size = unzReadCurrentFile(unz_file, sz_read_buffer, BUFFER_SIZE);
            if (read_file_size < 0) {
                unzCloseCurrentFile(unz_file);
                delete[] data;
                return -3;
            } else if (read_file_size == 0) {
                unzCloseCurrentFile(unz_file);
                break;
            } else {
                //写入读取的内存
                memcpy(data, sz_read_buffer, read_file_size);
                data += read_file_size;
            }
        }
        unzGoToNextFile(unz_file);
        file_map.insert(std::make_pair(std::string(sz_read_buffer), data));
    }
    //关闭
    if (unz_file) {
        unzClose(unz_file);
    }
    return 0;
}

bool ZipUtil::IsFolderExist(const char *const folder_path) {
    return 0 == access(folder_path, F_OK);
}

/**
 * 判断文件是否存在
 */
bool ZipUtil::IsFileExist(const char *const &filepath) {
    return 0 == access(filepath, F_OK);
}

/**
 * 获取文件大小
 */
long ZipUtil::GetFileSize(const char *const &filepath) {
    FILE *fp = fopen(filepath, "rb");
    if (fp == nullptr) {
        return -1;
    }
    fseek(fp, 0, SEEK_END);
    long file_size = ftell(fp);
    fclose(fp);
    return file_size;
}

bool ZipUtil::CreateDirIfNotExist(const char *const &folder) {
    // 如果目录不存在则创建
    if (0 != ::access(folder, 0)) {
        if (0 != ::mkdir(folder, 0755)) {
            return false;
        }
    }
    return true;
}