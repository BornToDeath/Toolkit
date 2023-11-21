//
// Created by lixiaoqing on 2021/6/22.
//

#ifndef UTILS_FILE_UTIL_H
#define UTILS_FILE_UTIL_H

#include <iostream>

namespace utils {

/**
 * 文件工具类
 */
namespace file_util {

using BYTE = unsigned char;

/**
 * 判断文件是否存在
 * @param filepath
 * @return
 */
bool IsFileExist(const char *filepath);

/**
 * 判断文件夹是否存在
 * @param folder_path
 * @return
 */
bool IsFolderExist(const char *folder_path);

/**
 * 获取文件 MD5 值
 * @param filepath
 * @return
 */
std::string GetFileMD5(const char *filepath);

/**
 * 目录若不存在则创建
 * @param dir
 * @return
 */
bool CreateDirIfNotExist(const char *const &dir);

/**
 * 获取文件大小
 * @param filepath
 * @return
 */
long GetFileSize(const char *const &filepath);

/**
 * 获取文件内容。注意：data 需要手动释放!
 * @param filepath 文件名
 * @param data     文件内容
 * @param length   文件长度
 */
void GetFileBytes(const char *filepath, BYTE **data, long &length);

/**
 * 将二进制数据写到文件中。
 *   文件存在: 覆盖
 *   文件不存在: 创建
 * @param filepath
 * @param data
 * @param length
 */
void WriteBinaryToFile(const char *filepath, const BYTE *data, int length);

/**
 * 将文本数据写入文件
 * @param filepath
 * @param text
 * @param isDelete true : 删除后写入
 *                 false: 追加写入
 */
bool WriteTextToFile(const std::string &filepath, const std::string &text, bool isDelete = true);

/**
 * 读取文件第一行
 * @param filepath
 * @return
 */
std::string GetFirstLine(const std::string &filepath);

/**
 * 删除指定目录下的所有文件。不能递归！
 * @param folder
 */
void DeleteAllFiles(const std::string &folder);

}  // namespace file_util
}  // namespace utils

#endif //UTILS_FILE_UTIL_H
