
# Protobuf 使用指南



## CMake 使用 Protobuf

CMake 使用 Protobuf 有三种方式：

1. protobuf_generate_cpp
2. protoc
3. Protobuf 源码

其中，前两种方法都需要本机已经安装 Protobuf ，并且网上也有现成的示例（如 [这里](https://blog.csdn.net/qq_37868450/article/details/113727764) ）。

本文使用第三种方式，即直接使用源码编译出来的 Protobuf 库文件。考虑到一些设备本身可能并没有 Protobuf 环境（特别是一些性能较差的智能硬件），那就需要使用源码编译出 Protobuf 的库文件和头文件的方式来使用 Protobuf ，并且这种方式与环境无关，灵活性较高。但是缺点是每次 proto 文件更新时都需要手动生成对应的 `.pb.h/cc` 文件。


## 环境要求

1. 要求 C++11 或更高。
2. 要求 Linux 系统。本工程使用的库是 Linux 环境下的 Protobuf 库，所以可以直接在 Linux 下运行。如果要在其他平台下运行，需要编译对应平台下的 Protobuf 库文件。

注：本工程使用的 Protobuf 的版本是 `v3.11.4` 。其他版本的库的编译方法可以参考 [这篇文章](https://blog.csdn.net/unonoi/article/details/131817640) 。


## 使用步骤

1. 首先需要有一份 proto 文件，然后根据此 proto 文件生成对应的 C++ 头文件和源文件，生成命令如下：

```shell
./protoc --proto_path=${SRC_DIR} --cpp_out=${DST_DIR} ${SRC_DIR}/Test.proto
```

详细的操作可参考此文档：[【ProtoBuf】ProtoBuf 通过 proto 文件生成 cc/h 文件](https://blog.csdn.net/unonoi/article/details/120006673) 。

2. 使用 `PbHelper` 类来进行 PB 相关的一些操作。

```c++
#include "pb_helper.h"

tutorial::Person person;
std::string s;

// pb 转 string
PbHelper::ToString(person, s);

// pb 转 json
PbHelper::ToJson(person, s);

// pb 转 二进制数据
PbHelper::ToBinary(person, s);

// 二进制数据 转 pb
PbHelper::ParseFromBinary(s, person);

// json 转 pb
PbHelper::ParseFromJson(s, person);
```



## Protobuf API 接口

Protobuf 提供的主要的序列化和反序列化的 API 接口：

```c++
// 序列化
bool SerializeToOstream(ostream* output) const;
bool SerializeToArray(void *data, int size) const;
bool SerializeToString(string* output) const;

// 反序列化
bool ParseFromIstream(istream* input);
bool ParseFromArray(const void* data, int size);
bool ParseFromString(const string& data);
```

Protobuf 序列化和反序列化都是二进制数据，所以 `SerializeToString() / ParseFromString()` 方法输出/输入的也是二进制数据，不可以使用 `DebugString()` 或可见的文件字符串。

其他（反）序列化接口的功能都与此类似。




## 其他

在链接 protobuf 库文件时需要添加 `-lpthread` 选项，否则可能会报错：

```shell
terminate called after throwing an instance of 'std::system_error'
  what():  Unknown error -1
Aborted
```