//
// Created by lixiaoqing on 2021/7/15.
//


#include <iostream>
#include "Common/CommonUtil.h"
#include "Device/DeviceUtil.h"


void DeviceUtilTest();


int main() {

    DeviceUtilTest();

    return 0;
}

void DeviceUtilTest() {

    // 测试：执行 shell
    std::string cmd = "pwd";
    auto res = DeviceUtil::executeCommand(cmd.c_str());
    CommonUtil::print("%s: %s", cmd.c_str(), res.c_str());

    cmd = "lixiaoqing";
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
    CommonUtil::print("总内存: %d KB", memTotal);

    // 测试：空闲内存大小
    auto memFree = DeviceUtil::getMemoryFree();
    CommonUtil::print("空闲内存: %d KB", memFree);

    // 测试：已用内存
    auto memUsage = DeviceUtil::getMemoryUsage();
    CommonUtil::print("已用内存: %d KB", memUsage);
}