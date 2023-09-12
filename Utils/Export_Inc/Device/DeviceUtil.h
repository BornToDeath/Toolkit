//
// Created by lixiaoqing on 2021/7/14.
//

#ifndef UTILS_DEVICEUTIL_H
#define UTILS_DEVICEUTIL_H

#include <string>

/**
 * 设备工具类
 */
namespace DeviceUtil {

    /**
     * 获取设备当前 CPU 温度。单位：摄氏度
     * @return
     */
    float getCPUTemperature();

    /**
     * 获取设备当前 GPU 温度。单位：摄氏度
     * @return
     */
    float getGPUTemperature();

    /**
     * 获取设备总内存大小。单位：B
     * @return
     */
    unsigned int getMemoryTotal();

    /**
     * 获取设备当前内存占用大小。单位：B
     * @return
     */
    unsigned int getMemoryUsage();

    /**
     * 获取设备当前可用内存大小。单位：B
     * @return
     */
    unsigned int getMemoryFree();

    /**
     * 执行 shell 命令
     * @param cmd
     * @return 命令执行结果
     */
    std::string executeCommand(const char *cmd);

    /**
     * 获取指定进程占用的内存。单位：B
     * @param pid
     * @return
     */
    unsigned int getMemoryUsageByPid(int pid);

    /**
     * 获取指定进程的进程名
     * @param pid
     * @return
     */
    std::string getNameByPid(int pid);

    /**
     * 获取逻辑 CPU 核数
     * @return
     */
    int getLogicalCpuCoreCount();

    /**
     * 获取 CPU 在线核数
     * @return
     */
    int getOnlineCpuCoreCount();

    /**
     * 获取 CPU Hardware
     * @return
     */
    std::string getCpuHardware();

    /**
     * 获取整体 CPU 占用率
     * @return
     */
    float getTotalCpuUsage();

    /**
     * 获取当前进程 CPU 占用率
     * @return
     */
    float getCurProcCpuUsage();

    // 仅限内部使用
    namespace internal {

        /**
         * 获取整体 CPU 占用时间
         * @return <total, idle>
         */
        std::pair<long, long> getTotalCpuTime();

        /**
         * 获取当前进程 CPU 占用时间
         * @return
         */
        long getCurProcCpuTime();
    }
};

#endif //UTILS_DEVICEUTIL_H
