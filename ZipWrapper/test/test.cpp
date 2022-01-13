//
// Created by lixiaoqing on 2022/1/12.
//

#include <iostream>
#include "ZipUtils.h"


namespace Test {
    void unzipFile() {
        const std::string zipFilePath = "/home/nvidia/AIDot/BaseLayer/ZipWrapper/test/test.zip";
        const std::string dstFolder = "/home/nvidia/AIDot/BaseLayer/ZipWrapper/test/folder1";
        bool isOk = ZipUtils::unZipFile(zipFilePath, dstFolder);
        if (isOk) {
            std::cout << "解压成功" << std::endl;
        } else {
            std::cout << "解压失败" << std::endl;
        }
    }
};


int main() {
    Test::unzipFile();
    return 0;
}