# 日志模块

[TOC]


# 序


## 日志模块的功能

日志模块具有的功能有：

1. 日志存储。单独的一个日志存储线程来做。
2. 日志加密。采用 base64 方式来进行加密。
   ~~3. 日志上传。单独的一个日志上传线程来做。~~


## 日志类型

现有的日志类型有：

* Normal
* Error

上述几种日志类型的含义见 `Log.h` 文件。

如果想要新增日志类型，只需：

1. 在 `Log.h` 的 `LogType` 枚举类中新增一种日志类型
2. 在 `std::string logtools::GetLogTypeName(LogType type_)` 方法中配置类型名，此类型名为对应类型的日志要存储的目录名
3. ~~在 `LogUploader.cpp` 中的适当位置为此类型的日志添加上传时机的代码~~


## 1. 使用方法


1. 引入头文件

```c++
#include "Log.h"
```

2. 进行初始化配置

```c++
/**
 * 日志初始化操作
 * 注意：必须在调用日志打印接口之前被调用！
 * @param logDir 日志文件的根目录
 */
static void Init(const char * const logDir);
```

3. 调用日志接口

如下方式调用：

```c++

Log::Debug(LogType type, const char *tag, const char *format, ...);

Log::Info(LogType type, const char *tag, const char *format, ...);

Log::Warn(LogType type, const char *tag, const char *format, ...);

Log::rror(LogType type, const char *tag, const char *format, ...);
```

如果是被项目内使用，则调用如下接口即可。此时，对于 *Debug()、Info()、Warn() 接口* ，`type` 默认为 `Normal` ，对于 *Error()* 接口，`type` 默认为 `Error` 。

```c++
Log::Debug(const char *tag, const char *format, ...);

Log::Info(const char *tag, const char *format, ...);

Log::Warn(const char *tag, const char *format, ...);

Log::Error(const char *tag, const char *format, ...);
```


参数说明：

`type` : 日志类型

`tag` : 标签，可以是类名或其他有意义的标签

`format` : 格式化日志内容


## 2. 实现思路


### 日志存储的整理流程


* 日志存储整体流程

优先采用 Mmap 的方式实现日志数据的存储，如果 Mmap 失败，则尝试切换到内存缓存的方式，如果内存缓存失败，则最终失败。

流程：

1. 在第一条日志传进来时，尝试 Mmap 映射，在映射前判断待映射的日志文件是否准备好。
2. 如果当前日志文件已满，则将当前日志文件重命名，并创建新的日志文件进行存储。在进行重命名之前会判断历史日志文件的总大小是否超出限制，如超出则删除超出的最早的历史日志文件。
3. 尝试进行 Mmap 映射，将日志文件和进程空间进行映射，映射长度是 2MB 。
4. 如果 Mmap 映射成功，则处理当前日志：一方面输出到标准输出，一方面写入文件。在写入文件之前需对日志内容进行加密。
5. 如果 Mmap 映射失败，尝试创建内存缓存（大小 4KB ）。如果内存缓存创建失败，则最终失败。
6. 对于之后的每条日志，转 2 。


### ~~日志上传的整理流程~~

* 日志上传整理流程

1. 当没有日志在上传时，启动日志上传线程进行日志上传处理
2. 压缩日志。将所有的日志文件压缩为一个 zip 文件
3. 上传日志。发送网络请求，上传 zip 日志压缩文件
4. 上传成功删除长传的历史日志文件和 zip 压缩文件
5. 重置上传标志位

注：

1. 对于 `Error` 类型的日志，设备启动之后主动上传；其他类型日志在接收到服务端下发的日志上传指令之后被动上传，服务端通过「轨迹上传接口」下发上传指令



## 3. 类说明


* 对外暴露的类

**Log** : 日志接口类。对外提供日志接口。

~~**LogUploader** : 日志上传类。对外提供日志上传接口~~


* 对外隐藏的类

**LogCommon** : 常量类。定义日志相关常量。

**LogData** : 日志数据结构类。

**LogEncryptor** : 日志加密类。实现日志内容的加密。

**Logger** : 日志实现类。实现日志处理的逻辑。

**LogTools** : 提供日志相关辅助功能，如获取系统当前时间戳、打印日志等方法。

**IMmap** : Mmap 抽象基类。提供 Mmap 映射相关函数。

**LogMmap** : Mmap 实现类。实现 Mmap 映射相关函数。

**LogStorageHelper** : 日志存储类。实现日志文件相关操作，包括日志文件重命名、日志文件删除等操作。

**LogModel** : 日志模型类，每个 model 对应一种日志类型。