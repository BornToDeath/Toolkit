//
// Created by lixiaoqing on 2021/6/22.
//

#ifndef UTILS_FILEUTIL_H
#define UTILS_FILEUTIL_H


#include <iostream>

/**
 * 文件工具类
 */
namespace FileUtil {

    using BYTE = unsigned char;

    /**
     * 判断文件是否存在
     * @param filePath
     * @return
     */
    bool isFileExist(const char *filePath);

    /**
     * 判断文件夹是否存在
     * @param folderPath
     * @return
     */
    bool isFolderExist(const char *folderPath);

    /**
     * 获取文件 MD5 值
     * @param filePath
     * @return
     */
    std::string getFileMD5(const char *filePath);

    /**
     * 目录若不存在则创建
     * @param dir
     * @return
     */
    bool createDirIfNotExist(const char *const &dir);

    /**
     * 获取文件大小
     * @param filePath
     * @return
     */
    long getFileSize(const char *const &filePath);

    /**
     * 获取文件内容。注意：data 需要手动释放!
     * @param filePath 文件名
     * @param data     文件内容
     * @param length   文件长度
     */
    void getFileBytes(const char *filePath, BYTE **data, long &length);

    /**
     * 将二进制数据写到文件中。
     *   文件存在: 覆盖
     *   文件不存在: 创建
     * @param filePath
     * @param data
     * @param length
     */
    void writeBinary2File(const char *filePath, const BYTE *data, int length);

    /**
     * 将文本数据写入文件
     * @param filePath
     * @param text
     * @param isDelete true : 删除后写入
     *                 false: 追加写入
     */
    bool writeText2File(const std::string &filePath, const std::string &text, bool isDelete = true);

    /**
     * 读取文件第一行
     * @param filePath
     * @return
     */
    std::string getFirstLine(const std::string &filePath);

    /**
     * 删除指定目录下的所有文件。不能递归！
     * @param folder
     */
    void deleteAllFiles(const std::string &folder);

};


#endif //UTILS_FILEUTIL_H
