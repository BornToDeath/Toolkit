//
// Created by lixiaoqing on 2021/7/14.
//

#include <iostream>
#include <fstream>
#include "Device/DeviceUtil.h"

#define TAG "DeviceUtil"


float DeviceUtil::getCPUTemperature() {
    const std::string filePath = "/sys/class/thermal/thermal_zone0/temp";
    std::ifstream input(filePath, std::ios::in);
    if (!input.is_open()) {
        return 0;
    }

    std::string line;
    getline(input, line);
    if (line.empty()) {
        return 0;
    }

    float temp;
    try {
        temp = std::stof(line) / 1000;
    } catch (...) {
        return 0;
    };
    return temp;
}

float DeviceUtil::getGPUTemperature() {
    // TODO 没有找到获取 GPU 温度的 API
    return 0;
}

int DeviceUtil::getMemoryTotal() {
    // 获取总内存大小
    const char *cmd = R"(cat /proc/meminfo | grep MemTotal | awk '{print $2}')";
    auto res = executeCommand(cmd);
    if (res.empty()) {
        return 0;
    }

    int mem = 0;
    try {
        mem = std::stoi(res);
    } catch (...) {
        return 0;
    };
    return mem;
}

int DeviceUtil::getMemoryUsage() {
    // 获取内存占用情况
    auto total = getMemoryTotal();
    auto free = getMemoryFree();
    if (total == 0 || free == 0) {
        return 0;
    }
    auto usage = total - free;
    return (usage > 0) ? usage : 0;
}

int DeviceUtil::getMemoryFree() {
    // 获取可用内存大小
    const char *cmd = R"(cat /proc/meminfo | grep MemFree | sed -n '1p' | awk '{print $2}')";
    auto res = executeCommand(cmd);
    if (res.empty()) {
        return 0;
    }

    int mem = 0;
    try {
        mem = std::stoi(res);
    } catch (...) {
        return 0;
    };
    return mem;
}


std::string DeviceUtil::executeCommand(const char *cmd) {
    std::string ret;

    // 执行 shell 命令
    auto file = popen(cmd, "r");
    if (file == nullptr) {
        return ret;
    }

    const int size = 1024;
    char temp[size] = {0};
    while (fgets(temp, size, file) != nullptr) {
        ret.append(temp);
    }
    pclose(file);

    ret = std::string(temp);
    return ret;
}
