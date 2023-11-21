//
// Created by lixiaoqing on 2021/7/14.
//

#ifndef UTILS_DEVICE_UTIL_H
#define UTILS_DEVICE_UTIL_H

#include <string>

namespace utils {

/**
 * 设备工具类
 */
namespace device_util {

/**
 * 获取设备当前 CPU 温度。单位：摄氏度
 * @return
 */
float GetCPUTemperature();

/**
 * 获取设备当前 GPU 温度。单位：摄氏度
 * @return
 */
float GetGPUTemperature();

/**
 * 获取设备总内存大小。单位：B
 * @return
 */
unsigned int GetMemoryTotal();

/**
 * 获取设备当前内存占用大小。单位：B
 * @return
 */
unsigned int GetMemoryUsage();

/**
 * 获取设备当前可用内存大小。单位：B
 * @return
 */
unsigned int GetMemoryFree();

/**
 * 执行 shell 命令
 * @param cmd
 * @return 命令执行结果
 */
std::string ExecuteCommand(const char *cmd);

/**
 * 获取指定进程占用的内存。单位：B
 * @param pid
 * @return
 */
unsigned int GetMemoryUsageByPid(int pid);

/**
 * 获取指定进程的进程名
 * @param pid
 * @return
 */
std::string GetNameByPid(int pid);

/**
 * 获取逻辑 CPU 核数
 * @return
 */
int GetLogicalCpuCoreCount();

/**
 * 获取 CPU 在线核数
 * @return
 */
int GetOnlineCpuCoreCount();

/**
 * 获取 CPU Hardware
 * @return
 */
std::string GetCpuHardware();

/**
 * 获取整体 CPU 占用率
 * @return
 */
float GetTotalCpuUsage();

/**
 * 获取当前进程 CPU 占用率
 * @return
 */
float GetCurProcCpuUsage();

// 仅限内部使用
namespace internal {

/**
 * 获取整体 CPU 占用时间
 * @return <total, idle>
 */
std::pair<long, long> GetTotalCpuTime();

/**
 * 获取当前进程 CPU 占用时间
 * @return
 */
long GetCurProcCpuTime();

}  // namespace internal
}  // namespace device_util
}  // namespace utils

#endif //UTILS_DEVICE_UTIL_H
