# RT-Thread Nano 配置

RT-Thread Nano 的配置在 rtconfig.h 中进行，通过开关宏定义来使能或关闭某些功能，接下来对该配置文件中的宏定义进行说明。

## 基础配置

1、设置系统最大优先级，可设置范围 8 到 256，默认值 32，可修改。

```c
#define RT_THREAD_PRIORITY_MAX  32
```
2、设置 RT-Thread 操作系统节拍，表示多少 tick 每秒，如默认值为 100 ，表示一个时钟节拍（os tick）长度为 10ms。常用值为 100 或 1000。时钟节拍率越快，系统的额外开销就越大。

```c
#define RT_TICK_PER_SECOND  1000
```
3、字节对齐时设定对齐的字节个数，默认 4，常使用 ALIGN(RT_ALIGN_SIZE) 进行字节对齐。

```c
#define RT_ALIGN_SIZE   4
```
4、设置对象名称的最大长度，默认 8 个字符，一般无需修改。

```c
#define RT_NAME_MAX    8
```
5、设置使用组件自动初始化功能，默认需要使用，开启该宏则可以使用自动初始化功能。

```c
#define RT_USING_COMPONENTS_INIT
```
6、开启 `RT_USING_USER_MAIN` 宏，则打开 user_main 功能，默认需要开启，这样才能调用 RT-Thread 的启动代码；main 线程的栈大小可修改。

```c
#define RT_USING_USER_MAIN
#define RT_MAIN_THREAD_STACK_SIZE     512
```
## 内核调试功能配置

定义 `RT_DEBUG` 宏则开启 debug 模式。若开启系统调试，则在实现打印之后可以打印系统 LOG 日志。请在代码开发与调试过程中打开该项，帮助调试定位问题，在代码发布时关闭该项。

```c
//#define RT_DEBUG                 // 关闭 debug

#define RT_DEBUG_INIT 0            // 启用组件初始化调试配置，设置为 1 则会打印自动初始化的函数名称

//#define RT_USING_OVERFLOW_CHECK  // 关闭栈溢出检查
```
## 钩子函数配置

设置是否使用钩子函数，默认关闭。

```c
//#define RT_USING_HOOK                         // 是否 开启系统钩子功能

//#define RT_USING_IDLE_HOOK                    // 是否 开启空闲线程钩子功能
```
## 软件定时器配置

设置是否启用软件定时器，以及相关参数的配置，默认关闭。

```c
#define RT_USING_TIMER_SOFT       0             // 关闭软件定时器功能，为 1 则打开
#if RT_USING_TIMER_SOFT == 0
#undef RT_USING_TIMER_SOFT
#endif

#define RT_TIMER_THREAD_PRIO        4           // 设置软件定时器线程的优先级，默认为 4

#define RT_TIMER_THREAD_STACK_SIZE  512         // 设置软件定时器线程的栈大小，默认为 512 字节
```
## IPC 配置

系统支持的 IPC 有：信号量、互斥量、事件集、邮箱、消息队列。通过定义相应的宏打开或关闭该 IPC 的使用。

```c
#define RT_USING_SEMAPHORE         // 设置是否使用 信号量，默认打开

//#define RT_USING_MUTEX           // 设置是否使用 互斥量

//#define RT_USING_EVENT           // 设置是否使用 事件集

#define RT_USING_MAILBOX           // 设置是否使用  邮箱

//#define RT_USING_MESSAGEQUEUE    // 设置是否使用 消息队列
```
## 内存配置

RT-Thread 内存管理包含：内存池、内存堆、小内存算法。通过开启相应的宏定义使用相应的功能。

```c
//#define RT_USING_MEMPOOL      // 是否使用 内存池

#define RT_USING_HEAP           // 是否使用 内存堆

#define RT_USING_SMALL_MEM      // 是否使用 小内存管理

//#define RT_USING_TINY_SIZE    // 是否使用 小体积的算法，牵扯到 rt_memset、rt_memcpy 所产生的体积
```

## FinSH 配置

当系统加入 FinSH 组件源码后，需要在 rtconfig.h 中开启以下项

```c
#include "finsh_config.h"
```
该头文件中包含了对 FinSH 组件的配置。如下是该头文件中包含的 FinSH 组件的配置项：

```c
/* 打开 FinSH 组件 */
#define RT_USING_FINSH

/* 使用 MSH 模式 */
#define FINSH_USING_MSH
#define FINSH_USING_MSH_ONLY

/* tshell 线程的优先级与线程栈大小 */
#define FINSH_THREAD_PRIORITY       21   // 请检查系统最大优先级的值，该值必须在系统支持的优先级范围之内
#define FINSH_THREAD_STACK_SIZE     1024

/* 使用符号表，使用命令描述 */
#define FINSH_USING_SYMTAB
#define FINSH_USING_DESCRIPTION
```


注意：若未加入 FinSH 组件源码，请勿开启此项。
## DEVICE 框架配置

当系统中加入 device 框架源码时，则需要在 rtconfig.h 中开启以下项

```c
#define RT_USING_DEVICE
```
开启该项则将加入 device 框架源码。

注意：若未加入 device 源码，请勿开启此项。

## 常见问题

Q：移植完成之后出现 hard fault。

A：在默认情况下，系统配置的各种线程栈大小均较小，若不能正常运行，很有可能是栈不够用，可将栈值调大。例如 main 线程栈大小默认为 256，在实际使用时，main 中可能加入其它代码导致栈不够用的情况；FinSH 组件的线程 tshell，默认栈 512 也比较小，在使用时可以调大。
