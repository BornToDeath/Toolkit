//
// Created by boss on 2023/11/22.
//

#include <gtest/gtest.h>

#include "common/common_util.h"
#include "device/device_util.h"

#define LOG(format, ...) printf(format "\n", ##__VA_ARGS__)

using namespace utils::device_util;
using namespace utils::common_util;

TEST(DeviceUtilTest, DeviceUtilTest) {
    // 测试：执行 shell
    std::string cmd = "pwd";
    auto res = ExecuteCommand(cmd.c_str());
    Print("%s: %s", cmd.c_str(), res.c_str());
    EXPECT_TRUE(res.empty() == false);

    cmd = "ls -l";
    res = ExecuteCommand(cmd.c_str());
    Print("%s: %s", cmd.c_str(), res.c_str());
    EXPECT_TRUE(res.empty() == false);

    // 测试：CPU 温度
    auto cpu_temp = GetCPUTemperature();
    Print("CPU 温度: %f", cpu_temp);
//    EXPECT_GT(cpu_temp, 0);

    // 测试：GPU 温度
    auto gpu_temp = GetGPUTemperature();
    Print("GPU 温度: %f", gpu_temp);

    // 测试：总内存大小
    auto mem_total = GetMemoryTotal();
    Print("总内存: %u B", mem_total);
    EXPECT_GT(mem_total, 0);

    // 测试：空闲内存大小
    auto mem_free = GetMemoryFree();
    Print("空闲内存: %u B", mem_free);
    EXPECT_GT(mem_free, 0);

    // 测试：已用内存
    auto mem_usage = GetMemoryUsage();
    Print("已用内存: %u B", mem_usage);
    EXPECT_GT(mem_usage, 0);

    int pid = getpid();
    auto cur_mem_usage = GetMemoryUsageByPid(pid);
    Print("当前进程(%d)占用内存: %u B", pid, cur_mem_usage);
    EXPECT_GT(cur_mem_usage, 0);

    auto proc_name = GetNameByPid(pid);
    Print("当前进程(%d)名: %s", pid, proc_name.c_str());
    EXPECT_TRUE(proc_name.empty() == false);

    auto total_core_count = GetLogicalCpuCoreCount();
    Print("逻辑核数: %d", total_core_count);
    EXPECT_GT(total_core_count, 0);

    auto online_core_count = GetOnlineCpuCoreCount();
    Print("在线核数: %d", online_core_count);
    EXPECT_GT(online_core_count, 0);

    auto cpu_hardware = GetCpuHardware();
    Print("CPU Hardware: %s", cpu_hardware.c_str());
//    EXPECT_TRUE(cpu_hardware.empty() == false);

    auto total_cpu_usage = GetTotalCpuUsage();
    Print("Total CPU Usage: %f", total_cpu_usage);
    EXPECT_GT(total_cpu_usage, 0);

    auto cur_cpu_usage = GetCurProcCpuUsage();
    Print("Current Process CPU Usage: %f", cur_cpu_usage);
    EXPECT_GT(total_cpu_usage, 0);
}