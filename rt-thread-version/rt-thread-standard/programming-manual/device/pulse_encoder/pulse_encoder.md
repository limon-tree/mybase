# Pulse Encoder 设备

## 脉冲编码器简介

脉冲编码器是利用光学、磁性或机械接点的方式感测位置，并将位置信息转换为电子信号后输出的传感器。其输出的电子信号一般被用作控制位置时的回授信号。

脉冲编码器按照工作原理可分为增量式和绝对式两类。增量式编码器是将位移转换成周期性的电信号，再把这个电信号转变成计数脉冲，用脉冲的个数表示位移的大小。绝对式编码器的每一个位置对应一个确定的数字码，因此它的示值只与测量的起始和终止位置有关，而与测量的中间过程无关。当代大多数的微控制器都提供了编码器外设用于接收存储脉冲编码器的信号。

## 访问脉冲编码器设备

应用程序通过 RT-Thread 提供的 I/O 设备管理接口来访问脉冲编码器设备，相关接口如下所示：

| **函数** | **描述**                           |
| -------------------- | ---------------------------------- |
| rt_device_find()  | 查找脉冲编码器设备  |
| rt_device_open()     | 打开脉冲编码器设备（仅支持只读的方式）  |
| rt_device_control()  | 控制脉冲编码器设备，可以清空计数值、获取类型、使能设备。  |
| rt_device_read()  | 获取脉冲编码器当前值  |
| rt_device_close()  | 关闭脉冲编码器设备  |

### 查找脉冲编码器设备

应用程序根据脉冲编码器的设备名称获取设备句柄，进而可以操作脉冲编码器设备，查找设备函数如下所示:

```c
rt_device_t rt_device_find(const char* name);
```

| **参数** | **描述**                           |
| -------- | ---------------------------------- |
| name     | 脉冲编码器的设备名称                           |
| **返回** | ——                                 |
| 脉冲编码器设备句柄 | 查找到对应设备将返回相应的设备句柄 |
| RT_NULL  | 没有找到设备             |

一般情况下，注册到系统的脉冲编码器的设备名称为 pulse1，pulse2等，使用示例如下所示：

```c
#define PULSE_ENCODER_DEV_NAME    "pulse1"    /* 脉冲编码器名称 */
rt_device_t pulse_encoder_dev;                /* 脉冲编码器设备句柄 */
/* 查找脉冲编码器设备 */
pulse_encoder_dev = rt_device_find(PULSE_ENCODER_DEV_NAME);
```

### 打开脉冲编码器设备

通过设备句柄，应用程序可以打开设备。打开设备时，会检测设备是否已经初始化，没有初始化则会默认调用初始化接口初始化设备。通过如下函数打开设备:

```c
rt_err_t rt_device_open(rt_device_t dev, rt_uint16_t oflags);
```

| **参数**   | **描述**                         |
| ---------- | ------------------------------- |
| dev        | 脉冲编码器设备句柄                        |
| oflags     | 设备打开模式，一般以只读方式打开，即取值：RT_DEVICE_OFLAG_RDONLY |
| **返回**   | ——                             |
| RT_EOK     | 设备打开成功                |
| 其他错误码 | 设备打开失败                |

使用示例如下所示：

```c
#define PULSE_ENCODER_DEV_NAME    "pulse1"    /* 脉冲编码器名称 */
rt_device_t pulse_encoder_dev;                /* 脉冲编码器设备句柄 */
/* 查找脉冲编码器设备 */
pulse_encoder_dev = rt_device_find(PULSE_ENCODER_DEV_NAME);
/* 以只读方式打开设备 */
rt_device_open(pulse_encoder_dev, RT_DEVICE_OFLAG_RDONLY);
```

### 控制脉冲编码器设备

通过命令控制字，应用程序可以对脉冲编码器设备进行设置，通过如下函数完成：

```c
rt_err_t rt_device_control(rt_device_t dev, rt_uint8_t cmd, void* arg);
```

| **参数**   | **描述**                                   |
| ---------- | ------------------------------------------ |
| dev        | 设备句柄                                   |
| cmd        | 命令控制字 |
| arg        | 控制的参数                                 |
| **返回**   | ——                                         |
| RT_EOK     | 函数执行成功                               |
| -RT_ENOSYS | 执行失败，dev 为空                          |
| 其他错误码 | 执行失败                                   |

脉冲编码器设备支持的命令控制字如下所示：

| **控制字**   | **描述**                                   |
| ---------------------- | ------------------------ |
| PULSE_ENCODER_CMD_GET_TYPE | 获取脉冲编码器类型 |
| PULSE_ENCODER_CMD_ENABLE   | 使能脉冲编码器 |
| PULSE_ENCODER_CMD_DISABLE  | 失能脉冲编码器  |
| PULSE_ENCODER_CMD_CLEAR_COUNT | 清空编码器计数值 |

### 读取脉冲编码器计数值

通过如下函数可以读取脉冲编码器计数值：

```c
rt_size_t rt_device_read(rt_device_t dev, rt_off_t pos, void* buffer, rt_size_t size);
```

| **参数**   | **描述**                                   |
| ---------- | ------------------------------------------ |
| dev                | 设备句柄                            |
| pos                | 固定值为 0  |
| buffer             | rt_int32_t 类型变量的地址，用于存放脉冲编码器的值。 |
| size               | 固定值为 1                 |
| **返回**           | ——                                 |
| 固定返回值  | 返回 1 |

使用示例如下所示：

```c
#define PULSE_ENCODER_DEV_NAME    "pulse1"    /* 脉冲编码器名称 */
rt_device_t pulse_encoder_dev;                /* 脉冲编码器设备句柄 */
rt_int32_t count;
/* 查找脉冲编码器设备 */
pulse_encoder_dev = rt_device_find(PULSE_ENCODER_DEV_NAME);
/* 以只读方式打开设备 */
rt_device_open(pulse_encoder_dev, RT_DEVICE_OFLAG_RDONLY);
/* 读取脉冲编码器计数值 */
rt_device_read(pulse_encoder_dev, 0, &count, 1);
```

### 关闭脉冲编码器设备

通过如下函数可以关闭脉冲编码器设备:

```c
rt_err_t rt_device_close(rt_device_t dev);
```

| **参数**   | **描述**                           |
| ---------- | ---------------------------------- |
| dev        | 脉冲编码器设备句柄                           |
| **返回**   | ——                                 |
| RT_EOK     | 关闭设备成功                       |
| -RT_ERROR  | 设备已经完全关闭，不能重复关闭设备 |
| 其他错误码 | 关闭设备失败                       |

 关闭设备接口和打开设备接口需配对使用，打开一次设备对应要关闭一次设备，这样设备才会被完全关闭，否则设备仍处于未关闭状态。

 使用示例如下所示：

```c
#define PULSE_ENCODER_DEV_NAME    "pulse1"    /* 脉冲编码器名称 */
rt_device_t pulse_encoder_dev;                /* 脉冲编码器设备句柄 */
/* 查找定时器设备 */
pulse_encoder_dev = rt_device_find(PULSE_ENCODER_DEV_NAME);
... ...
rt_device_close(pulse_encoder_dev);
```

## 脉冲编码器设备使用示例

脉冲编码器设备的具体使用方式可以参考如下示例代码，示例代码的主要步骤如下：

1. 首先根据脉冲编码器的设备名称 “pulse1” 查找设备获取设备句柄。

2. 以只读方式打开设备 “pulse1” 。

3. 读取脉冲编码器设备的计数值。

4. 清空脉冲编码器的计数值。（可选步骤）

```c
/*
 * 程序清单：这是一个脉冲编码器设备使用例程
 * 例程导出了 pulse_encoder_sample 命令到控制终端
 * 命令调用格式：pulse_encoder_sample
 * 程序功能：每隔 500 ms 读取一次脉冲编码器外设的计数值，然后清空计数值，将读取到的计数值打印出来。
*/

#include <rtthread.h>
#include <rtdevice.h>

#define PULSE_ENCODER_DEV_NAME    "pulse1"    /* 脉冲编码器名称 */

static int pulse_encoder_sample(int argc, char *argv[])
{
    rt_err_t ret = RT_EOK;
    rt_device_t pulse_encoder_dev = RT_NULL;   /* 脉冲编码器设备句柄 */
    rt_uint32_t index;
    rt_int32_t count;

    /* 查找脉冲编码器设备 */
    pulse_encoder_dev = rt_device_find(PULSE_ENCODER_DEV_NAME);
    if (pulse_encoder_dev == RT_NULL)
    {
        rt_kprintf("pulse encoder sample run failed! can't find %s device!\n", PULSE_ENCODER_DEV_NAME);
        return RT_ERROR;
    }

    /* 以只读方式打开设备 */
    ret = rt_device_open(pulse_encoder_dev, RT_DEVICE_OFLAG_RDONLY);
    if (ret != RT_EOK)
    {
        rt_kprintf("open %s device failed!\n", PULSE_ENCODER_DEV_NAME);
        return ret;
    }

    for (index = 0; index <= 10; index ++)
    {
        rt_thread_mdelay(500);
        /* 读取脉冲编码器计数值 */
        rt_device_read(pulse_encoder_dev, 0, &count, 1);
        /* 清空脉冲编码器计数值 */
        rt_device_control(pulse_encoder_dev, PULSE_ENCODER_CMD_CLEAR_COUNT, RT_NULL);
        rt_kprintf("get count %d\n",count);
    }

    rt_device_close(pulse_encoder_dev);
    return ret;
}
/* 导出到 msh 命令列表中 */
MSH_CMD_EXPORT(pulse_encoder_sample, pulse encoder sample);
```
