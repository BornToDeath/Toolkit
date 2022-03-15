
//#include <zconf.h>
#include <unistd.h>
#include <cstring>
#include <sys/stat.h>
#include "ZipUtils.h"
#include "zip.h"
#include "unzip.h"

#define TAG "ZipUtils"
#define log(format, args...) printf(TAG " | " format, ##args); printf("\n")

bool ZipUtils::unZipFile(const std::string &zipFilePath, const std::string &destFolder) {

    if (zipFilePath.empty() || destFolder.empty()) {
        log("zipFilePath.empty() || dstFolder.empty()");
        return false;
    }

    if (!ZipUtils::isFileExist(zipFilePath.c_str())) {
        log("zip压缩文件不存在：%s", zipFilePath.c_str());
        return false;
    }

    if (!ZipUtils::isFolderExist(destFolder.c_str())) {
        log("解压到的目录不存在：%s", destFolder.c_str());
        return false;
    }

    // 1. 打开zip文件
    unzFile zipfile = unzOpen(zipFilePath.c_str());
    if (zipfile == nullptr) {
        log("unzOpen failed");
        return false;
    }

    // 2. 获取zip文件的信息
    unz_global_info global_info;
    if (unzGetGlobalInfo(zipfile, &global_info) != UNZ_OK) {
        log("unzGetGlobalInfo failed");
        unzClose(zipfile);
        return false;
    }

    // 压缩文件中的文件个数
    const int fileCount = static_cast<int>(global_info.number_entry);

    // 3. 解析zip文件
    for (int i = 0; i < fileCount; ++i) {
        unz_file_info64 file_info64;
        char filename[512] = {0};

        // 解析当前内部文件信息
        int isOk = unzGetCurrentFileInfo64(zipfile, &file_info64, filename, sizeof(filename), nullptr, 0, nullptr, 0);
        if (isOk != UNZ_OK) {
            // 只要有一个文件解析失败，就终止解压
            log("unzGetCurrentFileInfo failed");
            unzClose(zipfile);
            return false;
        }

        char name[1024] = {0};
        if (filename[strlen(filename) - 1] == '/') {
//            log("这是一个文件夹：%s", filename);
            sprintf(name, "%s/%s", destFolder.c_str(), filename);
            if (!ZipUtils::createDirIfNotExist(name)) {
                log("创建文件夹失败：%s", filename);
            }
        } else {
            if (unzOpenCurrentFile(zipfile) != UNZ_OK) {
                log("unzOpenCurrentFile failed");
                unzClose(zipfile);
                return false;
            }

            sprintf(name, "%s/%s", destFolder.c_str(), filename);
            if (writeData(zipfile, name) < 0) {
                log("writeData failed");
                unzCloseCurrentFile(zipfile);
                unzClose(zipfile);
                return false;
            }
        }

        unzCloseCurrentFile(zipfile);
        unzGoToNextFile(zipfile);
    }

    unzClose(zipfile);
    return true;
}

int ZipUtils::writeData(unzFile &zipfile, const char *destPathName) {
    if (destPathName == nullptr) {
        return -1;
    }

    // 删除以.开头的文件
    std::string s_path = destPathName;
    int index = (int) s_path.find_last_of('/');
    std::string name = s_path.substr(index + 1);
    if (name.find('.') == 0) {
        return 0;
    }

    FILE *fp = fopen(destPathName, "wb");
    if (fp == nullptr) {
        return -1;
    }

    char read_buffer[1024] = {0};
    int error = UNZ_OK;

    do {
        // 读取文件
        error = unzReadCurrentFile(zipfile, read_buffer, sizeof(read_buffer));
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

bool ZipUtils::unZipFileByName(const std::string &zipFilePath, const std::string &fileName,
                               const std::string &destFolder) {
    if (zipFilePath.empty() || fileName.empty()) {
        log("zipFilePath.empty() || fileName.empty()");
        return false;
    }
    if (!isFileExist(zipFilePath.c_str())) {
        log("压缩文件不存在：%s", zipFilePath.c_str());
        return false;
    }
    if (!isFolderExist(destFolder.c_str())) {
        log("解压到的目录不存在：%s", destFolder.c_str());
        return false;
    }

    // 1. 打开zip文件
    unzFile fileHandle = unzOpen(zipFilePath.c_str());
    if (fileHandle == nullptr) {
        log("unzOpen failed");
        return false;
    }

    // 2.1 定位到指定文件
    if (unzLocateFile(fileHandle, fileName.c_str(), 1) != UNZ_OK) {
        log("unzLocateFile failed of the file: %s", fileName.c_str());
        return false;
    }

    // 2.2 获取当前内部压缩文件的信息
    unz_file_info64 file_info64;
    char fileNameBuf[512] = {0};
    int isOk = unzGetCurrentFileInfo64(fileHandle, &file_info64, fileNameBuf, sizeof(fileNameBuf), nullptr, 0,
                                       nullptr, 0);
    if (isOk != UNZ_OK) {
        log("unzGetCurrentFileInfo failed");
        unzClose(fileHandle);
        return false;
    }

    // 2.3 打开当前内部压缩文件
    if (unzOpenCurrentFile(fileHandle) != UNZ_OK) {
        log("unzOpenCurrentFile failed");
        unzClose(fileHandle);
        return false;
    }

    // 2.4 解析当前文件到指定文件夹
    char name[1024] = {0};
    sprintf(name, "%s/%s", destFolder.c_str(), fileNameBuf);
    if (writeData(fileHandle, name) < 0) {
        log("writeData failed");
        unzCloseCurrentFile(fileHandle);
        unzClose(fileHandle);
        return false;
    }

    unzCloseCurrentFile(fileHandle);
    unzClose(fileHandle);
    return true;
}

bool ZipUtils::unZipFileByNames(const std::string &zipFilePath, const std::vector<std::string> &fileNames,
                                const std::string &destFolder) {
    if (zipFilePath.empty() || fileNames.empty()) {
        log("zipFilePath.empty() || fileNames.empty()");
        return false;
    }
    if (!isFileExist(zipFilePath.c_str())) {
        log("压缩文件不存在：%s", zipFilePath.c_str());
        return false;
    }
    if (!isFolderExist(destFolder.c_str())) {
        log("解压到的目录不存在：%s", destFolder.c_str());
        return false;
    }

    // 1. 打开zip文件
    unzFile fileHandle = unzOpen(zipFilePath.c_str());
    if (fileHandle == nullptr) {
        log("unzOpen failed");
        return false;
    }

    // 2. 遍历文件名列表，循环解析文件
    for (const std::string &fileName: fileNames) {
        // 2.1 定位到指定文件
        if (unzLocateFile(fileHandle, fileName.c_str(), 1) != UNZ_OK) {
            log("unzLocateFile failed of the file: %s", fileName.c_str());
            return false;
        }

        // 2.2 获取当前内部压缩文件的信息
        unz_file_info64 file_info64;
        char fileNameBuf[512] = {0};
        int isOk = unzGetCurrentFileInfo64(fileHandle, &file_info64, fileNameBuf, sizeof(fileNameBuf), nullptr, 0,
                                           nullptr, 0);
        if (isOk != UNZ_OK) {
            // 只要有一个文件解析失败，就终止解压
            log("unzGetCurrentFileInfo failed");
            unzClose(fileHandle);
            return false;
        }

        // 2.3 打开当前内部压缩文件
        if (unzOpenCurrentFile(fileHandle) != UNZ_OK) {
            log("unzOpenCurrentFile failed");
            unzClose(fileHandle);
            return false;
        }

        // 2.4 解析当前文件到指定文件夹
        char name[1024] = {0};
        sprintf(name, "%s/%s", destFolder.c_str(), fileNameBuf);
        if (writeData(fileHandle, name) < 0) {
            log("writeData failed");
            unzCloseCurrentFile(fileHandle);
            unzClose(fileHandle);
            return false;
        }

        unzCloseCurrentFile(fileHandle);

        // 2.5 转到下个文件
        unzGoToNextFile(fileHandle);
    }

    unzClose(fileHandle);
    return true;
}

int ZipUtils::unZipFileInMemory(std::string zipFilePath, std::map<std::string, unsigned char *> &fileMap) {

    int nReturnValue;
    //打开zip文件
    unzFile unzfile = unzOpen(zipFilePath.c_str());
    if (unzfile == NULL) {
        return -1;
    }
    //获取zip文件 记得释放
    unz_global_info *pGlobalInfo = new unz_global_info;
    nReturnValue = unzGetGlobalInfo(unzfile, pGlobalInfo);
    if (nReturnValue != UNZ_OK) {
        return -1;
    }
    //解析zip文件
    unz_file_info ufi;
    unz_file_info *pFileInfo = &ufi;
    const int MAX_PATH = 256;
    //存放从zip中解析出来的内部文件
    char szZipFName[MAX_PATH];
    for (int i = 0; i < pGlobalInfo->number_entry; i++) {
        //解析得到zip中的文件信息
        nReturnValue = unzGetCurrentFileInfo(unzfile, pFileInfo, szZipFName, MAX_PATH, NULL, 0, NULL, 0);
        if (nReturnValue != UNZ_OK) {
            return -1;
        }
        // 当做文件夹处理
        if (pFileInfo->uncompressed_size == 0) {
            // 数据集中没有文件夹 都是文件
            continue;
        }
        //打开文件
        nReturnValue = unzOpenCurrentFile(unzfile);
        if (nReturnValue != UNZ_OK) {
            return -3;
        }
        unsigned long length = pFileInfo->uncompressed_size;
        // 记得内存释放
        unsigned char *data = new unsigned char[length];
        //读取文件
        const int BUFFER_SIZE = 4096;
        char szReadBuffer[BUFFER_SIZE];
        while (true) {
            memset(szReadBuffer, 0, BUFFER_SIZE);
            int nReadFileSize = unzReadCurrentFile(unzfile, szReadBuffer, BUFFER_SIZE);
            if (nReadFileSize < 0) {
                unzCloseCurrentFile(unzfile);
                delete[] data;
                return -3;
            } else if (nReadFileSize == 0) {
                unzCloseCurrentFile(unzfile);
                break;
            } else {
                //写入读取的内存
                memcpy(data, szReadBuffer, nReadFileSize);
                data += nReadFileSize;
            }
        }
        unzGoToNextFile(unzfile);
        fileMap.insert(std::make_pair(std::string(szReadBuffer), data));
    }
    //关闭
    if (unzfile) {
        unzClose(unzfile);
    }
    return 0;
}

bool ZipUtils::isFolderExist(const char *const folderPath) {
    return 0 == access(folderPath, F_OK);
}

/**
 * 判断文件是否存在
 */
bool ZipUtils::isFileExist(const char *const &filePath) {
    return 0 == access(filePath, F_OK);
}

/**
 * 获取文件大小
 */
long ZipUtils::getFileSize(const char *const &filePath) {
    FILE *fp = fopen(filePath, "rb");
    if (fp == nullptr) {
        return -1;
    }
    fseek(fp, 0, SEEK_END);
    long fileSize = ftell(fp);
    fclose(fp);
    return fileSize;
}

bool ZipUtils::createDirIfNotExist(const char *const &folder) {
    // 如果目录不存在则创建
    if (0 != ::access(folder, 0)) {
        if (0 != ::mkdir(folder, 0755)) {
            return false;
        }
    }
    return true;
}