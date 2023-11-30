#!/bin/bash

##############################
# 单元测试
##############################

cd $(dirname $0) || exit

# ========== 一些开关 ==========

# 编译环境开关
BUILD_DEBUG="yes"

# 单元测试开关
UNIT_TEST="yes"

# 覆盖率统计开关
COVERAGE="yes"

# ============================

build_params=""

if [ ${BUILD_DEBUG} == "yes" ]; then
  build_params+=" -DCMAKE_BUILD_TYPE=DEBUG"
fi

if [ ${UNIT_TEST} == "yes" ]; then
  build_params+=" -DTHREAD_POOL_TEST=ON"
fi

if [ ${COVERAGE} == "yes" ]; then
  build_params+=" -DCOVERAGE=ON"
fi

# 读取旧的 MD5
MD5_PATH="./build/CMakeLists.txt.md5"

if [ -f ${MD5_PATH} ]; then
  oldMD5="$(cat ${MD5_PATH})"
else
  oldMD5=""
fi

# 是否重新编译
if [ "$1" == "ON" ]; then
  oldMD5=""
fi

echo "旧的MD5=${oldMD5}"

# 计算当前的 MD5
newMD5="$(md5sum ../CMakeLists.txt | awk '{print $1}')"
echo "新的MD5=${newMD5}"

# 如果 MD5 相等，不需要重新 cmake
if [ "${oldMD5}" == "${newMD5}" ]; then
  cd ./build || exit
else
  echo "MD5不相等，正在重新编译..."
  rm -rf ./build && mkdir ./build && cd ./build || exit
  cmake ${build_params} ../.. || (echo "cmake失败！" && exit)

  # 保存新的 MD5
  touch CMakeLists.txt.md5 || (echo "有问题！" && exit)
  echo "${newMD5}" >CMakeLists.txt.md5
fi

echo "============================================================="
echo "PWD=$(pwd)"

# 编译
make -j8 || exit

echo "============================================================="

if [ ${BUILD_DEBUG} == "yes" ]; then
  function onSigInt() {
    echo " ( Ctrl+C ) "
  }

  # 捕获键盘 Ctrl+C
  trap onSigInt INT

  # 运行
  chmod +x ./run_thread_pool_unittest && ./run_thread_pool_unittest || exit
fi

echo "============================================================="

# 统计覆盖率
if [ ${COVERAGE} == "yes" ]; then
  echo "覆盖率统计..."
  rm -rf ./result/
  lcov --capture --directory ./ --output-file all.info --quiet || exit
  # 移除不需要统计的文件
  lcov --remove all.info '/usr/include/*' '*/test/*' '*/gtest/*' --quiet -o coverage.info || exit
  # 可视化
  genhtml coverage.info --output-directory ./coverage/ || exit
fi
