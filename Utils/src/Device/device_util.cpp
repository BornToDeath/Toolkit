//
// Created by lixiaoqing on 2021/7/14.
//

#include "device/device_util.h"

#include <unistd.h>

#include <cstring>
#include <fstream>
#include <iostream>
#include <numeric>
#include <vector>

#define TAG "device_util"
#define CPU_SLEEP_DURATION (3)

namespace utils {
namespace device_util {

float GetCPUTemperature() {
    // thermal/thermal_zoneX/type 为 "cpu" 的才是 cpu 的温度
    const std::string filepath = "/sys/class/thermal/thermal_zone0/temp";
    std::ifstream input(filepath, std::ios::in);
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

float GetGPUTemperature() {
    // TODO 没有找到获取 GPU 温度的 API
    return 0;
}

unsigned int GetMemoryTotal() {
#if 0
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
#else
    uint32_t value = 0;

    char buff[1024] = {0};
    FILE *fd = fopen("/proc/meminfo", "r");
    if (fd == nullptr) {
        return 0;
    }
    fread(buff, sizeof(buff), 1, fd);
    fclose(fd);

    char tmp1[256] = {0};
    char tmp2[256] = {0};
    char *p = strstr(buff, "MemTotal");
    if (p == nullptr) {
        return 0;
    }

    sscanf(p, "%s %u %s", tmp1, &value, tmp2);
    return value * 1024;
#endif
}

unsigned int GetMemoryUsage() {
    // 获取内存占用情况
    auto total = GetMemoryTotal();
    auto free = GetMemoryFree();
    if (total == 0 || free == 0) {
        return 0;
    }
    auto usage = total - free;
    return (usage > 0) ? usage : 0;
}

unsigned int GetMemoryFree() {
#if 0
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
#else
    uint32_t value = 0;

    char buff[1024] = {0};
    FILE *fd = fopen("/proc/meminfo", "r");
    if (fd == nullptr) {
        return 0;
    }
    fread(buff, sizeof(buff), 1, fd);
    fclose(fd);

    char tmp1[256] = {0};
    char tmp2[256] = {0};
    char *p = strstr(buff, "MemFree");
    if (p == nullptr) {
        return 0;
    }

    sscanf(p, "%s %u %s", tmp1, &value, tmp2);
    return value * 1024;
#endif
}


std::string ExecuteCommand(const char *cmd) {
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

unsigned int GetMemoryUsageByPid(int pid) {
    uint32_t value = 0;
    char filepath[1024];
    sprintf(filepath, "/proc/%d/status", pid);

    char buff[1024] = {0};
    FILE *fd = fopen(filepath, "r");
    if (fd == nullptr) {
        return 0;
    }
    fread(buff, sizeof(buff), 1, fd);
    fclose(fd);

    char tmp1[256] = {0};
    char tmp2[256] = {0};
    char *p = strstr(buff, "VmRSS");
    if (p == nullptr) {
        return 0;
    }

    sscanf(p, "%s %u %s", tmp1, &value, tmp2);
    return value * 1024;
}

std::string GetNameByPid(int pid) {
    char filepath[64]{};
    sprintf(filepath, "/proc/%d/comm", pid);
    std::ifstream ifs(filepath, std::ios::in);
    if (!ifs.is_open()) {
        return "";
    }

    std::string line;
    getline(ifs, line);
    ifs.close();

    if (!line.empty()) {
        line.erase(0, line.find_first_not_of(' '));
        line.erase(line.find_last_not_of(' ') + 1);
    }
    return line;
}

int GetLogicalCpuCoreCount() {
    int core_count = 0;

    const std::string filepath = "/proc/cpuinfo";
    std::ifstream ifs(filepath, std::ios::in);
    if (!ifs.is_open()) {
        return core_count;
    }

    std::string line;
    while (getline(ifs, line)) {
        if (line.find("processor") != std::string::npos) {
            ++core_count;
        }
    }
    ifs.close();

    return core_count;
}

int GetOnlineCpuCoreCount() {
    int core_count = GetLogicalCpuCoreCount();

    const std::string filepath = "/sys/devices/system/cpu/online";
    std::ifstream ifs(filepath, std::ios::in);
    if (!ifs.is_open()) {
        return core_count;
    }

    std::string line;
    getline(ifs, line);
    ifs.close();
    if (line.empty()) {
        return core_count;
    }

    line.erase(0, line.find_first_not_of(' '));
    line.erase(line.find_last_not_of(' ') + 1);

    int count = 0;
    int i = 0, j = 0;
    while (j != std::string::npos) {
        j = line.find(',', i);

        std::string ele;
        if (j == std::string::npos) {
            ele = line.substr(i);
        } else {
            ele = line.substr(i, j - i);
        }
        if (ele.empty()) {
            break;
        }

        int k;
        if ((k = ele.find('-')) == std::string::npos) {
            ++count;
        } else {
            auto str1 = ele.substr(0, k);
            auto str2 = ele.substr(k + 1);
            try {
                int num1 = std::stoi(str1);
                int num2 = std::stoi(str2);
                if (num1 <= num2) {
                    count += num2 - num1 + 1;
                }
            } catch (...) {
            }
        }
        i = j + 1;
    }

    core_count = count;
    return core_count;
}

std::string GetCpuHardware() {
    const std::string filepath = "/proc/cpuinfo";
    std::ifstream ifs(filepath, std::ios::in);
    if (!ifs.is_open()) {
        return "";
    }

    std::string line;
    while (getline(ifs, line)) {
        // android -> "Hardware"; linux -> "model name"
        if (line.find("Hardware") == std::string::npos &&
            line.find("model name") == std::string::npos) {
            continue;
        }
        auto index = line.find(':');
        if (index == std::string::npos) {
            continue;
        }
        line = line.substr(index + 1);
        if (line.empty()) {
            continue;
        }
        // 去除首尾空格
        line.erase(0, line.find_first_not_of(' '));
        line.erase(line.find_last_not_of(' ') + 1);
        break;
    }
    ifs.close();
    return line;
}

float GetTotalCpuUsage() {
    auto begin_cpu_time = internal::GetTotalCpuTime();
    sleep(CPU_SLEEP_DURATION);
    auto end_cpu_time = internal::GetTotalCpuTime();
    auto idle_usage =
            (end_cpu_time.second - begin_cpu_time.second) * 1.0f / (end_cpu_time.first - begin_cpu_time.first);
    return 1 - idle_usage;
}

float GetCurProcCpuUsage() {
    auto begin_total_cpu_time = internal::GetTotalCpuTime();
    auto begin_proc_cpu_time = internal::GetCurProcCpuTime();
    sleep(CPU_SLEEP_DURATION);
    auto end_proc_cpu_time = internal::GetCurProcCpuTime();
    auto end_total_cpu_time = internal::GetTotalCpuTime();
    auto usage =
            (end_proc_cpu_time - begin_proc_cpu_time) * 1.0f / (end_total_cpu_time.first - begin_total_cpu_time.first);
    return usage * GetLogicalCpuCoreCount();
}

std::pair<long, long> internal::GetTotalCpuTime() {
    FILE *fd = fopen("/proc/stat", "r");
    if (fd == nullptr) {
        return {};
    }

    char buff[1024] = {0};
    fgets(buff, sizeof(buff), fd);
    fclose(fd);

    char name[16];
    std::vector<long> params(10, 0);
    sscanf(buff, "%s %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld",
           name,
           &params[0], &params[1], &params[2], &params[3], &params[4],
           &params[5], &params[6], &params[7], &params[8], &params[9]);

//    user = params[0];
//    nice = params[1];
//    system = params[2];
    auto idle = params[3];
    auto total = std::accumulate(params.begin(), params.end(), 0L);
    return {total, idle};
}

long internal::GetCurProcCpuTime() {
    char filepath[64]{};
    sprintf(filepath, "/proc/%d/stat", getpid());
    std::string cpu_model;
    std::ifstream ifs(filepath, std::ios::in);
    if (!ifs.is_open()) {
        return 0;
    }

    std::string line;
    getline(ifs, line);
    ifs.close();
    if (line.empty()) {
        return 0;
    }
    int pos = -1;
    for (int i = 0; i < 13; ++i) {
        if ((pos = line.find(' ', pos + 1)) == std::string::npos) {
            return 0;
        }
    }

    auto info = line.substr(pos + 1);
    int utime = 0, stime = 0, cutime = 0, cstime = 0;
    sscanf(info.c_str(), "%d %d %d %d", &utime, &stime, &cutime, &cstime);
    return utime + stime + cutime + cstime;
}

}  // namespace device_util
}  // namespace utils