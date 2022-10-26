
# RingBuffer

无锁环形缓冲区，数据存放在堆上，适用于单生产者单消费者场景。



# LockFreeQueue

无锁队列，数据可以存放在堆上也可以存放在共享内存中。既适用于单生产者单消费者场景，又适用于多生产者多消费者场景。核心是 `spinlock` 这个数据结构，其保证了多线程下的线程安全。

说明：
- 如果在创建 LockFreeQueue 对象时没有指定 `name` ，则 LockFreeQueue 等同于 RingBuffer 。如果指定 `name` ，LockFreeQueue 其实还是环形缓冲区，只是底层采用共享内存而非堆内存来存储数据。
- `USE_LOCK` : 使用 CAS 实现的锁机制。如果不启用此宏，则 LockFreeQueue 仅适用于单生产者单消费者模式；如果启用，则 LockFreeQueue 适用于多生产者多消费者模式。
- `USE_POT` : 开启队列大小的 2 的幂对齐。如果不启用此宏，则队列的大小即为创建 LockFreeQueue 时指定的大小；如果启用，则队列的大小为2的幂次方，目的是方便计算队列头尾索引。
- `USE_MEM_BAR` : 启用内存屏障。


参考：[C++性能优化（十三）——无锁队列](https://blog.51cto.com/quantfabric/2588193)