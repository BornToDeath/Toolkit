
#ifndef ZIPWRAPPER_ZIPUTILS_H
#define ZIPWRAPPER_ZIPUTILS_H

#include <map>
#include <string>
#include <unzip.h>
#include <vector>

class ZipUtils final {

public:

    /**
     * 解压 Zip 文件
     * @param zipFilePath 待解压的zip压缩文件
     * @param destFolder  要解压到的文件目录
     * @return
     */
    static bool unZipFile(const std::string &zipFilePath, const std::string &destFolder);

    /**
     * 解压 Zip 文件中指定名称的单个文件
     * 注意：待解压的文件必须是在压缩文件的根目录下，否则解压失败！
     * @param zipFilePath
     * @param fileName
     * @param destFolder
     * @return
     */
    static bool unZipFileByName(const std::string &zipFilePath, const std::string &fileName,
                                const std::string &destFolder);

    /**
     * 解压 Zip 文件中的若干指定名称的文件
     * 注意：待解压的文件必须是在压缩文件的根目录下，否则解压失败！
     * @param zipFilePath
     * @param destFolder
     * @return
     */
    static bool unZipFileByNames(const std::string &zipFilePath, const std::vector<std::string> &fileNames,
                                 const std::string &destFolder);

    /**
     * 在内存中解压
     * @param zipFilePath
     * @param fileMap
     * @return
     */
    int unZipFileInMemory(std::string zipFilePath, std::map<std::string, unsigned char *> &fileMap);

    /**
     * 判断目录是否存在
     * @param folderPath
     * @return
     */
    static bool isFolderExist(const char *folderPath);

    /**
     * 判断文件是否存在
     */
    static bool isFileExist(const char *const &filePath);

    /**
     * 获取文件的大小
     */
    static long getFileSize(const char *const &filePath);

    /**
     * 目录不存在则创建
     * @param dir
     * @return
     */
    static bool createDirIfNotExist(const char *const &folder);

private:

    static int writeData(unzFile &zipfile, const char *destPathName);

};

#endif //ZIPWRAPPER_ZIPUTILS_H
