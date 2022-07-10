实验：信号的使用
================

实验目的
--------

-   理解信号的概念

-   使用信号进行线程间通信

-   在 RT-Thread 中熟练使用信号来完成需求

实验原理及程序结构
------------------

信号（又称为软中断信号），在软件层次上是对中断机制的一种模拟，在原理上，一个线程收到一个信号与处理器收到一个中断请求是类似的。

###  实验设计

本实验使用的例程为：[signal_sample.c](https://github.com/RT-Thread-packages/kernel-sample/blob/v0.2.0/signal_sample.c)

实验设计一个线程 thread1，并安装信号解除阻塞，通过给该线程发送信号，信号会触发线程执行一个软中断函数。

## 源程序说明

### 示例代码框架

RT-Thread 示例代码都通过 MSH_CMD_EXPORT 将示例初始函数导出到 msh 命令，可以在系统运行过程中，通过在控制台输入命令来启动。

### 示例源码

定义了线程用到的优先级、线程栈、时间片等参数：

```c
#include <rtthread.h>

#define THREAD_PRIORITY         25
#define THREAD_STACK_SIZE       512
#define THREAD_TIMESLICE        5

static rt_thread_t tid1 = RT_NULL;
```

线程 1 的入口函数，在线程 thread1 中安装信号并取消阻塞，运行一个打印计数的循环：

```c
/* 线程 1 的信号处理函数 */
void thread1_signal_handler(int sig)
{
    rt_kprintf("thread1 received signal %d\n", sig);
}

/* 线程 1 的入口函数 */
static void thread1_entry(void *parameter)
{
    int cnt = 0;

    /* 安装信号 */
    rt_signal_install(SIGUSR1, thread1_signal_handler);
    rt_signal_unmask(SIGUSR1);

    /* 运行 10 次 */
    while (cnt < 10)
    {
        /* 线程 1 采用低优先级运行，一直打印计数值 */
        rt_kprintf("thread1 count : %d\n", cnt);

        cnt++;
        rt_thread_mdelay(100);
    }
}
```

信号示例函数，创建线程 tread1，300ms 后发送信号，并将函数使用 MSH_CMD_EXPORT 导出命令。

```c
/* 信号示例的初始化 */
int signal_sample(void)
{
    /* 创建线程 1 */
    tid1 = rt_thread_create("thread1",
                            thread1_entry, RT_NULL,
                            THREAD_STACK_SIZE,
                            THREAD_PRIORITY, THREAD_TIMESLICE);

    if (tid1 != RT_NULL)
        rt_thread_startup(tid1);

    rt_thread_mdelay(300);

    /* 发送信号 SIGUSR1 给线程 1 */
    rt_thread_kill(tid1, SIGUSR1);

    return 0;
}

/* 导出到 msh 命令列表中 */
MSH_CMD_EXPORT(signal_sample, signal sample);
```

编译、运行和观察示例应用输出
----------------------------

编译工程，然后开始仿真。使用控制台 UART\#1 做为 msh 终端，可以看到系统的启动日志，输入 dynmem_sample 命令启动示例应用，示例输出结果如下：

```c
\ | /
- RT - Thread Operating System
/ | \ 3.1.0 build Aug 24 2018
2006 - 2018 Copyright by rt-thread team
msh >signal_sample
thread1 count : 0
thread1 count : 1
thread1 count : 2
msh >thread1 received signal 10
thread1 count : 3
thread1 count : 4
thread1 count : 5
thread1 count : 6
thread1 count : 7
thread1 count : 8
thread1 count : 9
```

例程中，SIGUSR1 定义为 10，首先线程安装信号并解除阻塞；然后发送信号给线程，信号会触发线程运行一个软中断函数。
