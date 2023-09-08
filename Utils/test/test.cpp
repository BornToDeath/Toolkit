//
// Created by lixiaoqing on 2021/7/15.
//


#include <iostream>
#include <unistd.h>
#include "Common/CommonUtil.h"
#include "Device/DeviceUtil.h"
#include "String/StringUtil.h"
#include "File/FileUtil.h"


namespace Test {
    void DeviceUtilTest() {
        // 测试：执行 shell
        std::string cmd = "pwd";
        auto res = DeviceUtil::executeCommand(cmd.c_str());
        CommonUtil::print("%s: %s", cmd.c_str(), res.c_str());

        cmd = "ls -l";
        res = DeviceUtil::executeCommand(cmd.c_str());
        CommonUtil::print("%s: %s", cmd.c_str(), res.c_str());

        // 测试：CPU 温度
        auto cpuTemp = DeviceUtil::getCPUTemperature();
        CommonUtil::print("CPU 温度: %f", cpuTemp);

        // 测试：GPU 温度
        auto gpuTemp = DeviceUtil::getGPUTemperature();
        CommonUtil::print("GPU 温度: %f", gpuTemp);

        // 测试：总内存大小
        auto memTotal = DeviceUtil::getMemoryTotal();
        CommonUtil::print("总内存: %u B", memTotal);

        // 测试：空闲内存大小
        auto memFree = DeviceUtil::getMemoryFree();
        CommonUtil::print("空闲内存: %u B", memFree);

        // 测试：已用内存
        auto memUsage = DeviceUtil::getMemoryUsage();
        CommonUtil::print("已用内存: %u B", memUsage);

        int pid = getpid();
        auto curMemUsage = DeviceUtil::getMemoryUsageByPid(pid);
        CommonUtil::print("当前进程(%d)占用内存: %u B", pid, curMemUsage);

        auto procName = DeviceUtil::getNameByPid(pid);
        CommonUtil::print("当前进程(%d)名: %s", pid, procName.c_str());

        auto totalCoreCount = DeviceUtil::getLogicalCpuCoreCount();
        CommonUtil::print("逻辑核数: %d", totalCoreCount);

        auto onlineCoreCount = DeviceUtil::getOnlineCpuCoreCount();
        CommonUtil::print("在线核数: %d", onlineCoreCount);

        auto cpuHardware = DeviceUtil::getCpuHardware();
        CommonUtil::print("CPU Hardware: %s", cpuHardware.c_str());
    }

    void StringUtilTest() {
        std::string str1 = "    sdasdasd";
        StringUtil::trim(str1);
        std::cout << "str:[" << str1 << "]" << std::endl;

        std::string str2 = "    sdasdasd     ";
        StringUtil::trim(str2);
        std::cout << "str:[" << str2 << "]" << std::endl;

        std::string str3 = "             ";
        StringUtil::trim(str3);
        std::cout << "str:[" << str3 << "]" << std::endl;

        std::string str4 = "";
        StringUtil::trim(str4);
        std::cout << "str:[" << str4 << "]" << std::endl;
    }

    void FileUtilTest() {
        char dir[512];
        getcwd(dir, sizeof(dir));
        const std::string filePath = std::string(dir) + "/Makefile";
        if (!FileUtil::isFileExist(filePath.c_str())) {
            CommonUtil::print("file not exist: %s", filePath.c_str());
        } else {
            auto md5 = FileUtil::getFileMD5(filePath.c_str());
            CommonUtil::print("md5: %s", md5.c_str());
        }
    }
}


int main() {
    Test::DeviceUtilTest();
    CommonUtil::print("%s", "-----------------------------------");
    Test::StringUtilTest();
    CommonUtil::print("%s", "-----------------------------------");
    Test::FileUtilTest();
    return 0;
}
