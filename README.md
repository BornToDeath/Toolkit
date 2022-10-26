# 说明


本工程基于 C++11 封装了一些常用的工具。

包括如下工具：

| 工具 | 描述 |
| :--- | :--- |
| Communication | 基于 HPSocket 实现的 Socket 通信机制 |
| Database | 数据库操作（增删改查），基于 SQLite3 |
| Json | Json 数据格式相关操作 |
| LockFreeQueue | 无锁队列 |
| Log | 日志功能，包括日志存储和加密 |
| Network | 网络相关 |
| NotificationCenter | 消息通知 |
| SQLite3 | SQLite 数据库，基于 sqlite3 三方库|
| SharedPreference | 共享存储，类似安卓的 SharedPreference |
| Thread | 线程相关 |
| ThreadDispatcher | 线程分发器 |
| ThreadPool | 一个非常简洁优雅的线程池 |
| Utils | 一些常用方法，包括字符串、文件、时间等 |
| ZipWrapper | 压缩/解压缩，基于 libzip 三方库 |

# 致谢

本工程的部分工具参考了一位很厉害的同事所写的代码：[木质的旋律](https://www.jianshu.com/u/7342e2f8da4f)