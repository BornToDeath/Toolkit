//
// Created by lixiaoqing on 2022/1/12.
//

#include <unistd.h>

#include <iostream>

#include "zip_util.h"

namespace Test {
void unzipFile() {
    char dir[128]{};
    getcwd(dir, sizeof(dir));
    const std::string zip_filepath = std::string(dir) + "/test.zip";
    const std::string dst_folder = std::string(dir) + "/zip";
    bool isOk = ZipUtil::UnzipFile(zip_filepath, dst_folder);
    if (isOk) {
        std::cout << "解压成功" << std::endl;
    } else {
        std::cout << "解压失败" << std::endl;
    }
}
}


int main() {
    Test::unzipFile();
    return 0;
}