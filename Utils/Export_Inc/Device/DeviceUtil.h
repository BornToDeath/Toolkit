//
// Created by lixiaoqing on 2021/7/14.
//

#ifndef UTILS_DEVICEUTIL_H
#define UTILS_DEVICEUTIL_H


/**
 * 设备工具类
 */
class DeviceUtil final {

public:

    /**
     * 获取设备当前 CPU 温度。单位：摄氏度
     * @return
     */
    static float getCPUTemperature();

    /**
     * 获取设备当前 GPU 温度。单位：摄氏度
     * @return
     */
    static float getGPUTemperature();

    /**
     * 获取设备总内存大小。单位：KB
     * @return
     */
    static int getMemoryTotal();

    /**
     * 获取设备当前内存占用大小。单位：KB
     * @return
     */
    static int getMemoryUsage();

    /**
     * 获取设备当前可用内存大小。单位：KB
     * @return
     */
    static int getMemoryFree();

    /**
     * 执行 shell 命令
     * @param cmd
     * @return 命令执行结果
     */
    static std::string executeCommand(const char *cmd);

public:

    DeviceUtil() = delete;

    ~DeviceUtil() = delete;

};

#endif //UTILS_DEVICEUTIL_H
