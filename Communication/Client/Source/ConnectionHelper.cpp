//
// Created by lixiaoqing on 2021/7/12.
//

#include <iostream>
#include <fstream>
#include <sstream>
#include "ConnectionHelper.h"
#include "File/FileUtil.h"
#include "Log.h"

#define TAG "ConnectionHelper"


bool ConnectionHelper::adbForward() {

    Log::info(TAG, "执行 adb forward 建立转发");

    // adb forward 脚本
    std::string cmd = R""(
#!/bin/bash

###################################
# adb forward 建立转发
###################################

# echo "======================= adb forward begin ======================="
# echo "提示: 需要以 su 用户执行此脚本!"

cd $(dirname $0) || exit
# echo "PWD=$(pwd)"

# 检测是否存在 Android 设备
res=$(adb devices | sed -n '2p')
if [ ! "${res}" ]; then
  echo "[info] adb devices is null!"
  # 重启 adb 服务
  echo "[info] 正在重启 adb 服务..."
  service udev restart || exit
  adb kill-server || exit
  adb start-server || exit
  # 再次检测
  res=$(adb devices | sed -n '2p')
  if [ ! "${res}" ]; then
      echo "[info] adb devices is still null!" && exit
  fi
  echo "[info] ----------- adb devices -----------"
  adb devices || exit
fi

# 建立 adb forward 转发
adb forward tcp:9001 tcp:9001

echo "[info] ----------- adb forward -----------"
adb forward --list

# echo "======================= adb forward end ======================="
)"";

    // 执行 shell 脚本
    ::system(cmd.c_str());
    return true;
}