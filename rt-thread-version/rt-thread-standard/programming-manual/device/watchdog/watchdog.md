# WATCHDOG 设备

## WATCHDOG 简介

硬件看门狗（watchdog timer）是一个定时器，其定时输出连接到电路的复位端。在产品化的嵌入式系统中，为了使系统在异常情况下能自动复位，一般都需要引入看门狗。

当看门狗启动后，计数器开始自动计数，在计数器溢出前如果没有被复位，计数器溢出就会对 CPU 产生一个复位信号使系统重启（俗称 “被狗咬”）。系统正常运行时，需要在看门狗允许的时间间隔内对看门狗计数器清零（俗称“喂狗“），不让复位信号产生。如果系统不出问题，程序能够按时“喂狗”。一旦程序跑飞，没有“喂狗”，系统“被咬” 复位。

一般情况下可以在 RT-Thread 的 idle 回调函数和关键任务里喂狗。

## 访问看门狗设备

应用程序通过 RT-Thread 提供的 I/O 设备管理接口来访问看门狗硬件，相关接口如下所示：

| **函数** | **描述**                           |
| ---------------- | ---------------------------------- |
| rt_device_find()  | 根据看门狗设备设备名称查找设备获取设备句柄      |
| rt_device_init()     | 初始化看门狗设备 |
| rt_device_control()   |控制看门狗设备 |
| rt_device_close()  | 关闭看门狗设备|

### 查找看门狗

应用程序根据看门狗设备名称获取设备句柄，进而可以操作看门狗设备，查找设备函数如下所示：

```c
rt_device_t rt_device_find(const char* name);
```

| **参数** | **描述**                           |
| -------- | ---------------------------------- |
| name     | 看门狗设备名称                           |
| **返回** | ——                                 |
| 设备句柄 | 查找到对应设备将返回相应的设备句柄 |
| RT_NULL  | 没有找到相应的设备对象             |

使用示例如下所示：

```c
#define WDT_DEVICE_NAME    "wdt"    /* 看门狗设备名称 */

static rt_device_t wdg_dev;         /* 看门狗设备句柄 */
/* 根据设备名称查找看门狗设备，获取设备句柄 */
wdg_dev = rt_device_find(WDT_DEVICE_NAME);
```

### 初始化看门狗

使用看门狗设备前需要先初始化，通过如下函数初始化看门狗设备:

```c
rt_err_t rt_device_init(rt_device_t dev);
```

| **参数**   | **描述**                         |
| ---------- | ------------------------------- |
| dev        | 看门狗设备句柄                        |
| **返回**   | ——                             |
| RT_EOK     | 设备初始化成功                |
| -RT_ENOSYS | 初始化失败，看门狗设备驱动初始化函数为空                          |
| 其他错误码 | 设备打开失败                |

使用示例如下所示：

```c
#define WDT_DEVICE_NAME    "wdt"    /* 看门狗设备名称 */

static rt_device_t wdg_dev;    /* 看门狗设备句柄 */
/* 根据设备名称查找看门狗设备，获取设备句柄 */
wdg_dev = rt_device_find(WDT_DEVICE_NAME);

/* 初始化设备 */
rt_device_init(wdg_dev);
```

### 控制看门狗

通过命令控制字，应用程序可以对看门狗设备进行配置，通过如下函数完成：

```c
rt_err_t rt_device_control(rt_device_t dev, rt_uint8_t cmd, void* arg);
```

| **参数**   | **描述**                                   |
| ---------- | ------------------------------------------ |
| dev        | 看门狗设备句柄                                   |
| cmd        | 命令控制字 |
| arg        | 控制的参数                                 |
| **返回**   | ——                                         |
| RT_EOK     | 函数执行成功                               |
| -RT_ENOSYS | 执行失败，dev 为空                          |
| 其他错误码 | 执行失败                                   |

命令控制字可取如下宏定义值：

```c
#define RT_DEVICE_CTRL_WDT_GET_TIMEOUT    (1) /* 获取溢出时间 */
#define RT_DEVICE_CTRL_WDT_SET_TIMEOUT    (2) /* 设置溢出时间 */
#define RT_DEVICE_CTRL_WDT_GET_TIMELEFT   (3) /* 获取剩余时间 */
#define RT_DEVICE_CTRL_WDT_KEEPALIVE      (4) /* 喂狗 */
#define RT_DEVICE_CTRL_WDT_START          (5) /* 启动看门狗 */
#define RT_DEVICE_CTRL_WDT_STOP           (6) /* 停止看门狗 */
```

设置看门狗溢出时间使用示例如下所示：

```c
#define WDT_DEVICE_NAME    "wdt"    /* 看门狗设备名称 */

rt_uint32_t timeout = 1;       /* 溢出时间，单位：秒*/
static rt_device_t wdg_dev;    /* 看门狗设备句柄 */
/* 根据设备名称查找看门狗设备，获取设备句柄 */
wdg_dev = rt_device_find(WDT_DEVICE_NAME);
/* 初始化设备 */
rt_device_init(wdg_dev);

/* 设置看门狗溢出时间 */
rt_device_control(wdg_dev, RT_DEVICE_CTRL_WDT_SET_TIMEOUT, &timeout);
/* 设置空闲线程回调函数 */
rt_thread_idle_sethook(idle_hook);
```

在空闲线程钩子函数里喂狗使用示例如下所示：

```c
static void idle_hook(void)
{
    /* 在空闲线程的回调函数里喂狗 */
    rt_device_control(wdg_dev, RT_DEVICE_CTRL_WDT_KEEPALIVE, NULL);
}
```

### 关闭看门狗

当应用程序完成看门狗操作后，可以关闭看门狗设备，通过如下函数完成:

```c
rt_err_t rt_device_close(rt_device_t dev);
```

| **参数**   | **描述**                           |
| ---------- | ---------------------------------- |
| dev        | 看门狗设备句柄                           |
| **返回**   | ——                                 |
| RT_EOK     | 关闭设备成功                       |
| -RT_ERROR  | 设备已经完全关闭，不能重复关闭设备 |
| 其他错误码 | 关闭设备失败                       |

关闭设备接口和打开设备接口需配对使用，打开一次设备对应要关闭一次设备，这样设备才会被完全关闭，否则设备仍处于未关闭状态。

## 看门狗设备使用示例

看门狗设备的具体使用方式可以参考如下示例代码，示例代码的主要步骤如下：

1. 根据设备名称 “wdt” 查找设备获取设备句柄。
2. 初始化设备后设置看门狗溢出时间。
3. 启动看门狗。
4. 喂狗：设置空闲线程回调函数，在空闲线程回调函数中喂狗。

```c
/*
 * 程序清单：这是一个独立看门狗设备使用例程
 * 例程导出了 wdt_sample 命令到控制终端
 * 命令调用格式：wdt_sample wdt
 * 命令解释：命令第二个参数是要使用的看门狗设备名称，为空则使用例程默认的看门狗设备。
 * 程序功能：程序通过设备名称查找看门狗设备，然后初始化设备并设置看门狗设备溢出时间。
 *           然后设置空闲线程回调函数，在回调函数里会喂狗。
*/

#include <rtthread.h>
#include <rtdevice.h>

#define WDT_DEVICE_NAME    "wdt"    /* 看门狗设备名称 */

static rt_device_t wdg_dev;         /* 看门狗设备句柄 */

static void idle_hook(void)
{
    /* 在空闲线程的回调函数里喂狗 */
    rt_device_control(wdg_dev, RT_DEVICE_CTRL_WDT_KEEPALIVE, NULL);
    rt_kprintf("feed the dog!\n ");
}

static int wdt_sample(int argc, char *argv[])
{
    rt_err_t ret = RT_EOK;
    rt_uint32_t timeout = 1;        /* 溢出时间，单位：秒 */
    char device_name[RT_NAME_MAX];

    /* 判断命令行参数是否给定了设备名称 */
    if (argc == 2)
    {
        rt_strncpy(device_name, argv[1], RT_NAME_MAX);
    }
    else
    {
        rt_strncpy(device_name, WDT_DEVICE_NAME, RT_NAME_MAX);
    }
    /* 根据设备名称查找看门狗设备，获取设备句柄 */
    wdg_dev = rt_device_find(device_name);
    if (!wdg_dev)
    {
        rt_kprintf("find %s failed!\n", device_name);
        return RT_ERROR;
    }

    /* 设置看门狗溢出时间 */
    ret = rt_device_control(wdg_dev, RT_DEVICE_CTRL_WDT_SET_TIMEOUT, &timeout);
    if (ret != RT_EOK)
    {
        rt_kprintf("set %s timeout failed!\n", device_name);
        return RT_ERROR;
    }
    /* 启动看门狗 */
    ret = rt_device_control(wdg_dev, RT_DEVICE_CTRL_WDT_START, RT_NULL);
    if (ret != RT_EOK)
    {
        rt_kprintf("start %s failed!\n", device_name);
        return -RT_ERROR;
    }
    /* 设置空闲线程回调函数 */
    rt_thread_idle_sethook(idle_hook);

    return ret;
}
/* 导出到 msh 命令列表中 */
MSH_CMD_EXPORT(wdt_sample, wdt sample);
```

