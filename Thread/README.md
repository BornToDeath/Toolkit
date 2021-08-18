
# Thread 模块说明

本模块提供如下功能：

* 常用线程
* 线程池
* 定时器


## 1. 线程


| 名称 | 描述 |
| :--- | :--- |
| Thread | 线程 |
| LoopThread | 循环线程 |
| HandlerThread | 消息线程 |


## 2. 线程池

头文件：`ThreadPool.h` ，使用静态方法创建, 调用 `shutDownAndDeleteSelf()` 销毁。


### 2.1 创建

```C++
auto *pool = ThreadPool::newThreadPool("线程池的名称", 3/*最大线程数*/);
```

### 2.2 添加任务

```C++
Runnable runnable = [] {
    // Do Somthing...
};

pool->execute(runnable);
```

### 2.3 释放空闲线程

```C++
pool->releaseIdleThreads();
```

### 2.3 销毁

```C++
pool->shutDownAndDeleteSelf();
pool = nullptr;
```


## 3. 定时器


定时器的本质是个线程。


### 3.1 创建定时器

```c++
#include "Timer/Timer.h"

Runnabkle runnable = []() {
    // xxx
};

Timer timer(runnable);
```

### 3.2 启动定时器

```c++
// 延迟执行任务
timer.startDelay(2000);

// 在将来的某个时刻执行任务
timer.startAt(timeMills);
```

### 3.3 销毁定时器

定时器执行完任务之后内部会自动销毁。