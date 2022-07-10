# HWTIMER 设备


## 定时器简介

硬件定时器一般有 2 种工作模式，定时器模式和计数器模式。不管是工作在哪一种模式，实质都是通过内部计数器模块对脉冲信号进行计数。下面是定时器的一些重要概念。

**计数器模式：**对外部输入引脚的外部脉冲信号计数。

**定时器模式：**对内部脉冲信号计数。定时器常用作定时时钟，以实现定时检测，定时响应、定时控制。

**计数器：**计数器可以递增计数或者递减计数。16位计数器的最大计数值为65535，32位的最大值为4294967295。

**计数频率：**定时器模式时，计数器单位时间内的计数次数，由于系统时钟频率是定值，所以可以根据计数器的计数值计算出定时时间，定时时间 = 计数值 / 计数频率。例如计数频率为 1MHz，计数器计数一次的时间则为 1 / 1000000， 也就是每经过 1 微秒计数器加一（或减一），此时 16 位计数器的最大定时能力为 65535 微秒，即 65.535 毫秒。

本定时器设备框架内部会自动处理硬件定时器超时的问题，例如16位定时器在1MHz的频率下最大只能维持65.535ms。但是在本定时器框架下，用户可以将定时器的溢出时间设置为例如500ms，框架内部会自动处理硬件溢出问题。当时间达到500ms后，框架会调用用户预先设置好的回调函数。

## 访问硬件定时器设备

应用程序通过 RT-Thread 提供的 I/O 设备管理接口来访问硬件定时器设备，相关接口如下所示：

| **函数** | **描述**                           |
| -------------------- | ---------------------------------- |
| rt_device_find()  | 查找定时器设备  |
| rt_device_open()     | 以读写方式打开定时器设备   |
| rt_device_set_rx_indicate()   | 设置超时回调函数 |
| rt_device_control()  | 控制定时器设备，可以设置定时模式（单次/周期）/计数频率，或者停止定时器  |
| rt_device_write()  | 设置定时器超时值，定时器随即启动  |
| rt_device_read()  | 获取定时器当前值  |
| rt_device_close()  | 关闭定时器设备  |

### 查找定时器设备

应用程序根据硬件定时器设备名称获取设备句柄，进而可以操作硬件定时器设备，查找设备函数如下所示:

```c
rt_device_t rt_device_find(const char* name);
```

| **参数** | **描述**                           |
| -------- | ---------------------------------- |
| name     | 硬件定时器设备名称                           |
| **返回** | ——                                 |
| 定时器设备句柄 | 查找到对应设备将返回相应的设备句柄 |
| RT_NULL  | 没有找到设备             |

一般情况下，注册到系统的硬件定时器设备名称为 timer0，timer1等，使用示例如下所示：

```c
#define HWTIMER_DEV_NAME   "timer0"     /* 定时器名称 */
rt_device_t hw_dev;                     /* 定时器设备句柄 */
/* 查找定时器设备 */
hw_dev = rt_device_find(HWTIMER_DEV_NAME);
```

### 打开定时器设备

通过设备句柄，应用程序可以打开设备。打开设备时，会检测设备是否已经初始化，没有初始化则会默认调用初始化接口初始化设备。通过如下函数打开设备:

```c
rt_err_t rt_device_open(rt_device_t dev, rt_uint16_t oflags);
```

| **参数**   | **描述**                         |
| ---------- | ------------------------------- |
| dev        | 硬件定时器设备句柄                        |
| oflags     | 设备打开模式，一般以读写方式打开，即取值：RT_DEVICE_OFLAG_RDWR |
| **返回**   | ——                             |
| RT_EOK     | 设备打开成功                |
| 其他错误码 | 设备打开失败                |

使用示例如下所示：

```c
#define HWTIMER_DEV_NAME   "timer0"     /* 定时器名称 */
rt_device_t hw_dev;                     /* 定时器设备句柄 */
/* 查找定时器设备 */
hw_dev = rt_device_find(HWTIMER_DEV_NAME);
/* 以读写方式打开设备 */
rt_device_open(hw_dev, RT_DEVICE_OFLAG_RDWR);
```

### 设置超时回调函数

通过如下函数设置定时器超时回调函数，当定时器超时将会调用此回调函数：

```c
rt_err_t rt_device_set_rx_indicate(rt_device_t dev, rt_err_t (*rx_ind)(rt_device_t dev,rt_size_t size))
```

| **参数**   | **描述**                         |
| ---------- | ------------------------------- |
| dev        | 设备句柄                        |
| rx_ind     | 超时回调函数，由调用者提供 |
| **返回**   | ——                             |
| RT_EOK     | 成功                |

使用示例如下所示：

```c
#define HWTIMER_DEV_NAME   "timer0"     /* 定时器名称 */
rt_device_t hw_dev;                     /* 定时器设备句柄 */

/* 定时器超时回调函数 */
static rt_err_t timeout_cb(rt_device_t dev, rt_size_t size)
{
    rt_kprintf("this is hwtimer timeout callback fucntion!\n");
    rt_kprintf("tick is :%d !\n", rt_tick_get());

    return 0;
}

static int hwtimer_sample(int argc, char *argv[])
{
    /* 查找定时器设备 */
    hw_dev = rt_device_find(HWTIMER_DEV_NAME);
    /* 以读写方式打开设备 */
    rt_device_open(hw_dev, RT_DEVICE_OFLAG_RDWR);
    
    ...
    
    /* 设置超时回调函数 */
    rt_device_set_rx_indicate(hw_dev, timeout_cb);

    return 0;
}
```

### 控制定时器设备

通过命令控制字，应用程序可以对硬件定时器设备进行配置，通过如下函数完成：

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

硬件定时器设备支持的命令控制字如下所示：

| **控制字**   | **描述**                                   |
| ---------------------- | ------------------------ |
| HWTIMER_CTRL_FREQ_SET | 设置计数频率（若未设置该项，默认为1Mhz 或 支持的最小计数频率） |
| HWTIMER_CTRL_STOP     | 停止定时器  |
| HWTIMER_CTRL_INFO_GET | 获取定时器特征信息  |
| HWTIMER_CTRL_MODE_SET | 设置定时器模式（若未设置，默认是HWTIMER_MODE_ONESHOT） |

获取定时器特征信息参数 arg 为指向结构体 struct rt_hwtimer_info 的指针，作为一个输出参数保存获取的信息。

> [!NOTE]
> 注：定时器硬件及驱动支持设置计数频率的情况下设置频率才有效，一般使用驱动设置的默认频率即可。

设置定时器模式时，参数 arg 可取如下值：

```c
HWTIMER_MODE_ONESHOT    单次定时
HWTIMER_MODE_PERIOD     周期性定时
```

设置定时器计数频率和定时模式的使用示例如下所示：

```c
#define HWTIMER_DEV_NAME   "timer0"     /* 定时器名称 */
rt_device_t hw_dev;                     /* 定时器设备句柄 */
rt_hwtimer_mode_t mode;         		/* 定时器模式 */
rt_uint32_t freq = 10000;       		/* 计数频率 */

static int hwtimer_sample(int argc, char *argv[])
{
    /* 查找定时器设备 */
    hw_dev = rt_device_find(HWTIMER_DEV_NAME);
    /* 以读写方式打开设备 */
    rt_device_open(hw_dev, RT_DEVICE_OFLAG_RDWR);

    ...

    /* 设置计数频率(若未设置该项，默认为1Mhz 或 支持的最小计数频率) */
    rt_device_control(hw_dev, HWTIMER_CTRL_FREQ_SET, &freq);
    /* 设置模式为周期性定时器（若未设置，默认是HWTIMER_MODE_ONESHOT）*/
    mode = HWTIMER_MODE_PERIOD;
    rt_device_control(hw_dev, HWTIMER_CTRL_MODE_SET, &mode);
    return 0;
}
```

### 设置定时器超时值

通过如下函数可以设置定时器的超时值，在调用该函数后，定时器更新参数并旋即开启。

```c
rt_size_t rt_device_write(rt_device_t dev, rt_off_t pos, const void* buffer, rt_size_t size);
```

| **参数**   | **描述**                                   |
| ---------- | ------------------------------------------ |
| dev                | 设备句柄                               |
| pos                | 写入数据偏移量，未使用，可取 0 值     |
| buffer             | 指向定时器超时时间结构体的指针       |
| size               | 超时时间结构体的大小                |
| **返回**           | ——                                     |
| 写入数据的实际大小    |  |
| 0                  | 失败  |

超时时间结构体原型如下所示：

```c
typedef struct rt_hwtimerval
{
    rt_int32_t sec;      /* 秒 s */
    rt_int32_t usec;     /* 微秒 us */
} rt_hwtimerval_t;
```

设置定时器超时值的使用示例如下所示：

```c
#define HWTIMER_DEV_NAME   "timer0"     /* 定时器名称 */
rt_device_t hw_dev;                     /* 定时器设备句柄 */
rt_hwtimerval_t timeout_s;      /* 定时器超时值 */

static int hwtimer_sample(int argc, char *argv[])
{
    /* 查找定时器设备 */
    hw_dev = rt_device_find(HWTIMER_DEV_NAME);
    /* 以读写方式打开设备 */
    rt_device_open(hw_dev, RT_DEVICE_OFLAG_RDWR);

	...

    /* 设置定时器超时值为5s并启动定时器 */
    timeout_s.sec = 5;      /* 秒 */
    timeout_s.usec = 0;     /* 微秒 */
    rt_device_write(hw_dev, 0, &timeout_s, sizeof(timeout_s));

    return 0;
}
```

### 获取定时器当前值

通过如下函数可以获取自定时器开始 (rt_device_write) 之后的运行时：

```c
rt_size_t rt_device_read(rt_device_t dev, rt_off_t pos, void* buffer, rt_size_t size);
```

| **参数**   | **描述**                                   |
| ---------- | ------------------------------------------ |
| dev                | 定时器设备句柄                               |
| pos                | 写入数据偏移量，未使用，可取 0 值     |
| buffer             | 输出参数，指向定时器超时时间结构体的指针       |
| size               | 超时时间结构体的大小                |
| **返回**           | ——                                     |
| 超时时间结构体的大小    | 成功 |
| 0                  | 失败  |

使用示例如下所示：

```c
rt_hwtimerval_t t;
/* 读取定时器开始之后的经过时间 */
rt_device_read(hw_dev, 0, &t, sizeof(t));
rt_kprintf("Read: Sec = %d, Usec = %d\n", t.sec, t.usec);
```

### 关闭定时器设备

通过如下函数可以关闭定时器设备:

```c
rt_err_t rt_device_close(rt_device_t dev);
```

| **参数**   | **描述**                           |
| ---------- | ---------------------------------- |
| dev        | 定时器设备句柄                           |
| **返回**   | ——                                 |
| RT_EOK     | 关闭设备成功                       |
| -RT_ERROR  | 设备已经完全关闭，不能重复关闭设备 |
| 其他错误码 | 关闭设备失败                       |

 关闭设备接口和打开设备接口需配对使用，打开一次设备对应要关闭一次设备，这样设备才会被完全关闭，否则设备仍处于未关闭状态。

 使用示例如下所示：

```c
#define HWTIMER_DEV_NAME   "timer0"     /* 定时器名称 */
rt_device_t hw_dev;                     /* 定时器设备句柄 */
/* 查找定时器设备 */
hw_dev = rt_device_find(HWTIMER_DEV_NAME);
... ...
rt_device_close(hw_dev);
```

> [!NOTE]
> 注：可能出现定时误差。假设计数器最大值 0xFFFF，计数频率 1Mhz，定时时间 1 秒又 1 微秒。由于定时器一次最多只能计时到 65535us，对于 1000001us 的定时要求。可以 50000us 定时 20 次完成，此时将会出现计算误差 1us。

## 硬件定时器设备完整使用示例

硬件定时器设备的具体使用方式可以参考如下示例代码，示例代码的主要步骤如下：

1. 首先根据定时器设备名称 “timer0” 查找设备获取设备句柄。

2. 以读写方式打开设备 “timer0” 。

3. 设置定时器超时回调函数。

4. 设置定时器模式为周期性定时器，并设置超时时间为 5 秒，此时定时器启动。

5. 延时 3500ms 后读取定时器时间，读取到的值会以秒和微秒的形式显示。

```c
/*
 * 程序清单：这是一个 hwtimer 设备使用例程
 * 例程导出了 hwtimer_sample 命令到控制终端
 * 命令调用格式：hwtimer_sample
 * 程序功能：硬件定时器超时回调函数周期性的打印当前tick值，2次tick值之差换算为时间等同于定时时间值。
*/

#include <rtthread.h>
#include <rtdevice.h>

#define HWTIMER_DEV_NAME   "timer0"     /* 定时器名称 */

/* 定时器超时回调函数 */
static rt_err_t timeout_cb(rt_device_t dev, rt_size_t size)
{
    rt_kprintf("this is hwtimer timeout callback fucntion!\n");
    rt_kprintf("tick is :%d !\n", rt_tick_get());

    return 0;
}

static int hwtimer_sample(int argc, char *argv[])
{
    rt_err_t ret = RT_EOK;
    rt_hwtimerval_t timeout_s;      /* 定时器超时值 */
    rt_device_t hw_dev = RT_NULL;   /* 定时器设备句柄 */
    rt_hwtimer_mode_t mode;         /* 定时器模式 */
    rt_uint32_t freq = 10000;       		/* 计数频率 */

    /* 查找定时器设备 */
    hw_dev = rt_device_find(HWTIMER_DEV_NAME);
    if (hw_dev == RT_NULL)
    {
        rt_kprintf("hwtimer sample run failed! can't find %s device!\n", HWTIMER_DEV_NAME);
        return RT_ERROR;
    }

    /* 以读写方式打开设备 */
    ret = rt_device_open(hw_dev, RT_DEVICE_OFLAG_RDWR);
    if (ret != RT_EOK)
    {
        rt_kprintf("open %s device failed!\n", HWTIMER_DEV_NAME);
        return ret;
    }

    /* 设置超时回调函数 */
    rt_device_set_rx_indicate(hw_dev, timeout_cb);

    /* 设置计数频率(若未设置该项，默认为1Mhz 或 支持的最小计数频率) */
    rt_device_control(hw_dev, HWTIMER_CTRL_FREQ_SET, &freq);
    /* 设置模式为周期性定时器（若未设置，默认是HWTIMER_MODE_ONESHOT）*/
    mode = HWTIMER_MODE_PERIOD;
    ret = rt_device_control(hw_dev, HWTIMER_CTRL_MODE_SET, &mode);
    if (ret != RT_EOK)
    {
        rt_kprintf("set mode failed! ret is :%d\n", ret);
        return ret;
    }

    /* 设置定时器超时值为5s并启动定时器 */
    timeout_s.sec = 5;      /* 秒 */
    timeout_s.usec = 0;     /* 微秒 */
    if (rt_device_write(hw_dev, 0, &timeout_s, sizeof(timeout_s)) != sizeof(timeout_s))
    {
        rt_kprintf("set timeout value failed\n");
        return RT_ERROR;
    }

    /* 延时3500ms */
    rt_thread_mdelay(3500);

    /* 读取定时器当前值 */
    rt_device_read(hw_dev, 0, &timeout_s, sizeof(timeout_s));
    rt_kprintf("Read: Sec = %d, Usec = %d\n", timeout_s.sec, timeout_s.usec);

    return ret;
}
/* 导出到 msh 命令列表中 */
MSH_CMD_EXPORT(hwtimer_sample, hwtimer sample);
```
