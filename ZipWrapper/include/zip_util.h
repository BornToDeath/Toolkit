
#ifndef ZIPWRAPPER_ZIP_UTIL_H
#define ZIPWRAPPER_ZIP_UTIL_H

#include <unzip.h>

#include <map>
#include <string>
#include <vector>

class ZipUtil final {

public:

    /**
     * 解压 Zip 文件
     * @param zip_filepath 待解压的zip压缩文件
     * @param dest_folder  要解压到的文件目录
     * @return
     */
    static bool UnzipFile(const std::string &zip_filepath, const std::string &dest_folder);

    /**
     * 解压 Zip 文件中指定名称的单个文件
     * 注意：待解压的文件必须是在压缩文件的根目录下，否则解压失败！
     * @param zip_filepath
     * @param filename
     * @param dest_folder
     * @return
     */
    static bool
    UnzipFileByName(const std::string &zip_filepath, const std::string &filename, const std::string &dest_folder);

    /**
     * 解压 Zip 文件中的若干指定名称的文件
     * 注意：待解压的文件必须是在压缩文件的根目录下，否则解压失败！
     * @param zip_filepath
     * @param dest_folder
     * @return
     */
    static bool UnzipFileByNames(const std::string &zip_filepath, const std::vector<std::string> &filenames,
                                 const std::string &dest_folder);

    /**
     * 在内存中解压
     * @param zip_filepath
     * @param file_map
     * @return
     */
    int UnzipFileInMemory(std::string zip_filepath, std::map<std::string, unsigned char *> &file_map);

    /**
     * 判断目录是否存在
     * @param folder_path
     * @return
     */
    static bool IsFolderExist(const char *folder_path);

    /**
     * 判断文件是否存在
     */
    static bool IsFileExist(const char *const &filepath);

    /**
     * 获取文件的大小
     */
    static long GetFileSize(const char *const &filepath);

    /**
     * 目录不存在则创建
     * @param dir
     * @return
     */
    static bool CreateDirIfNotExist(const char *const &folder);

private:

    static int WriteData(unzFile &zip_file, const char *dest_path_name);

};

#endif //ZIPWRAPPER_ZIP_UTIL_H
