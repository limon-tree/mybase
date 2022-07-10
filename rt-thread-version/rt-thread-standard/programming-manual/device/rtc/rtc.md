# RTC 设备

## RTC 简介

RTC （Real-Time Clock）实时时钟可以提供精确的实时时间，它可以用于产生年、月、日、时、分、秒等信息。目前实时时钟芯片大多采用精度较高的晶体振荡器作为时钟源。有些时钟芯片为了在主电源掉电时还可以工作，会外加电池供电，使时间信息一直保持有效。

RT-Thread 的 RTC设备为操作系统的时间系统提供了基础服务。面对越来越多的 IoT 场景，RTC 已经成为产品的标配，甚至在诸如 SSL 的安全传输过程中，RTC 已经成为不可或缺的部分。


## 访问 RTC 设备

应用程序通过 RTC 设备管理接口来访问 RTC 硬件，相关接口如下所示：

| **函数** | **描述**                           |
| ------------- | ---------------------------------- |
| set_date()  | 设置日期，年、月、日（当地时区）   |
| set_time()     | 设置时间，时、分、秒（当地时区）    |
| time()   | 获取时间戳（格林威治时间） |
| stime()   | 设置时间戳（格林威治时间） |

### 设置日期

通过如下函数设置 RTC 设备当前当地时区日期值：

```c
rt_err_t set_date(rt_uint32_t year, rt_uint32_t month, rt_uint32_t day)
```

| **参数** | **描述**                           |
| -------- | ---------------------------------- |
|year      |待设置生效的年份|
|month     |待设置生效的月份|
|day       |待设置生效的日|
| **返回** | ——                                 |
| RT_EOK   | 设置成功 |
| -RT_ERROR | 失败，没有找到 rtc 设备             |
| 其他错误码  | 失败                |

使用示例如下所示：

```c
/* 设置日期为2018年12月3号 */
set_date(2018, 12, 3);
```

### 设置时间

通过如下函数设置 RTC 设备当前当地时区时间值：

```c
rt_err_t set_time(rt_uint32_t hour, rt_uint32_t minute, rt_uint32_t second)
```

| **参数**   | **描述**                         |
| ---------- | ------------------------------- |
|hour         |待设置生效的时|
|minute        |待设置生效的分|
|second        |待设置生效的秒|
| **返回**   | ——                             |
| RT_EOK   | 设置成功 |
| -RT_ERROR | 失败，没有找到 rtc 设备             |
| 其他错误码  | 失败                |

使用示例如下所示：

```c
/* 设置时间为11点15分50秒 */
set_time(11, 15, 50);
```

### 获取当前时间

使用到 C 标准库中的时间 API 获取时间戳（格林威治时间）：

```c
time_t time(time_t *t)
```

| **参数**   | **描述**                         |
| ---------- | ------------------------------- |
|t          |时间数据指针     |
| **返回**   | ——                             |
| 当前时间值   |  |

使用示例如下所示：

```c
time_t now;     /* 保存获取的当前时间值 */
/* 获取时间 */
now = time(RT_NULL);
/* 打印输出时间信息 */
rt_kprintf("%s\n", ctime(&now));
```

> [!NOTE]
> 注：目前系统内只允许存在一个 RTC 设备，且名称为 `"rtc"` 。

## 功能配置

### 启用 Soft RTC （软件模拟 RTC）

在 menuconfig 中可以启用使用软件模拟 RTC 的功能，这个模式非常适用于对时间精度要求不高，没有硬件 RTC 的产品。配置选项如下所示：

```c
RT-Thread Components →
    Device Drivers:
        -*- Using RTC device drivers                                 /* 使用 RTC 设备驱动 */
        [ ]   Using software simulation RTC device                   /* 使用软件模拟 RTC */
```

### 启用 NTP 时间自动同步

如果 RT-Thread 已接入互联网，可启用 NTP 时间自动同步功能，定期同步本地时间。

首先在 menuconfig 中按照如下选项开启 NTP 功能：

```c
RT-Thread online packages →
    IoT - internet of things →
        netutils: Networking utilities for RT-Thread:
             [*]   Enable NTP(Network Time Protocol) client
```

开启 NTP 后 RTC 的自动同步功能将会自动开启，还可以设置同步周期和首次同步的延时时间：

```c
RT-Thread Components →
    Device Drivers:
        -*- Using RTC device drivers                                 /* 使用 RTC 设备驱动 */
        [ ]   Using software simulation RTC device                   /* 使用软件模拟 RTC */
        [*]   Using NTP auto sync RTC time                           /* 使用 NTP 自动同步 RTC 时间 */
       (30)    NTP first sync delay time(second) for network connect /* 首次执行 NTP 时间同步的延时。延时的目的在于，给网络连接预留一定的时间，尽量提高第一次执行 NTP 时间同步时的成功率。默认时间为 30S； */
       (3600)  NTP auto sync period(second)                          /* NTP 自动同步周期，单位为秒，默认一小时（即 3600S）同步一次。 */
```

```c
注意：在 2021-05-09 更新 RT-Thread 内核源码中，已经移除该部分
```



## FinSH 命令

输入 `date` 即可查看当前当地时区时间，大致效果如下：

```c
msh />date
Fri Feb 16 01:11:56 2018
msh />
```

同样使用 `date` 命令，在命令后面再依次输入 ` 年 ` ` 月 ` ` 日 ` ` 时 ` ` 分 ` ` 秒 ` （中间空格隔开, 24H 制），设置当前当地时区时间为 2018-02-16 01:15:30，大致效果如下：

```c
msh />date 2018 02 16 01 15 30
msh />
```

## RTC 设备使用示例

RTC 设备的具体使用方式可以参考如下示例代码，首先设置了年月日时分秒信息，然后延时 3 秒后获取当前时间信息。

```c
/*
 * 程序清单：这是一个 RTC 设备使用例程
 * 例程导出了 rtc_sample 命令到控制终端
 * 命令调用格式：rtc_sample
 * 程序功能：设置RTC设备的日期和时间，延时一段时间后获取当前时间并打印显示。
*/

#include <rtthread.h>
#include <rtdevice.h>

static int rtc_sample(int argc, char *argv[])
{
    rt_err_t ret = RT_EOK;
    time_t now;

    /* 设置日期 */
    ret = set_date(2018, 12, 3);
    if (ret != RT_EOK)
    {
        rt_kprintf("set RTC date failed\n");
        return ret;
    }

    /* 设置时间 */
    ret = set_time(11, 15, 50);
    if (ret != RT_EOK)
    {
        rt_kprintf("set RTC time failed\n");
        return ret;
    }

    /* 延时3秒 */
    rt_thread_mdelay(3000);

    /* 获取时间 */
    now = time(RT_NULL);
    rt_kprintf("%s\n", ctime(&now));

    return ret;
}
/* 导出到 msh 命令列表中 */
MSH_CMD_EXPORT(rtc_sample, rtc sample);
```

## Alarm 功能

### Alarm 简介

alarm 闹钟功能是基于 RTC 设备实现的，根据用户设定的闹钟时间，当时间到时触发 alarm 中断，执行闹钟事件，在硬件上 RTC 提供的 Alarm 是有限的，RT-Thread 将 Alarm 在软件层次上封装成了一个组件，原理上可以实现无限个闹钟，但每个闹钟只有最后一次设定有效

注意：要使用 Alarm 组件，需要使用 menuconfig 配置打开 Alarm 组件功能，由于alarm 是基于 RTC 设备，因此 RTC 也需要打开，且需要实现 RTC提供的 ops 接口中的`set_alarm` 和 `get_alarm` 接口，具体实现过程请参考 RTC 驱动开发文档

alarm 组件开启位置 ： RT-Thread Components -> Device Drivers -> Using RTC alarm

```c
RT-Thread Components  --->                   
    Device Drivers  ---> 
        [*]   Using RTC alarm
```

### 访问 Alarm 设备

应用程序通过使用 alarm 组件提供的接口使用 Alarm 功能，相关接口如下 :

| 函数            | 描述 |
| --------------- | ---- |
| rt_alarm_create() | 创建闹钟 |
|rt_alarm_start()|启动闹钟|
|rt_alarm_stop()|停止闹钟|
|rt_alarm_delete()|删除闹钟|
|rt_alarm_control()|控制 alarm 设备|
|rt_alarm_dump()|打印显示设置的闹钟信息|

#### 创建闹钟

通过如下函数创建 alarm 闹钟：

```c
rt_alarm_t rt_alarm_create(rt_alarm_callback_t callback, struct rt_alarm_setup *setup);
```

| 参数     | 描述                           |
| -------- | ------------------------------ |
| callback | 用户自定义的处理闹钟事件的函数 |
|setup|设置闹钟定时的日期时间，以及闹钟模式|
|返回|——|
|成功|rt_alarm 结构体对象句柄|
|失败|RT_NULL|

rt_alarm_setup 结构体原型如下 ：

```c
struct rt_alarm_setup
{
    rt_uint32_t flag;                /* alarm flag */
    struct tm wktime;                /* when will the alarm wake up user */
};
```

flag 闹钟模式有如下几种设置：

```c
#define RT_ALARM_ONESHOT       0x000 /* only alarm onece */
#define RT_ALARM_DAILY         0x100 /* alarm everyday */
#define RT_ALARM_WEEKLY        0x200 /* alarm weekly at Monday or Friday etc. */
#define RT_ALARM_MONTHLY       0x400 /* alarm monthly at someday */
#define RT_ALARM_YAERLY        0x800 /* alarm yearly at a certain date */
#define RT_ALARM_HOUR          0x1000 /* alarm each hour at a certain min:second */
#define RT_ALARM_MINUTE        0x2000 /* alarm each minute at a certain second */
#define RT_ALARM_SECOND        0x4000 /* alarm each second */
```


注意: 闹钟创建后并不会启动。


#### 启动闹钟

通过如下函数启动 alarm 闹钟:

```c
rt_err_t rt_alarm_start(rt_alarm_t alarm);
```

| 参数  | 描述                               |
| ----- | ---------------------------------- |
| alarm | 创建闹钟的 rt_alarm 结构体对象句柄 |
|返回|——|
|成功|RT_EOK|
|失败|-RT_ERROR|

#### 停止闹钟

通过如下函数停止 alarm 闹钟：

```c
rt_err_t rt_alarm_stop(rt_alarm_t alarm);
```

| 参数 | 描述 |
| ---- | ---- |
| alarm | 创建闹钟的 rt_alarm 结构体对象句柄 |
|返回|——|
|成功|RT_EOK|
|失败|-RT_ERROR|

#### 删除闹钟

通过如下函数删除 alarm 闹钟：

```c
rt_err_t rt_alarm_delete(rt_alarm_t alarm);
```

| 参数 | 描述 |
| ---- | ---- |
| alarm | 创建闹钟的 rt_alarm 结构体对象句柄 |
|返回| ——                                 |
|成功|RT_EOK|
|失败|RT_ERROR|

#### 控制闹钟

通过如下函数控制 alarm 闹钟：

```c
rt_err_t rt_alarm_control(rt_alarm_t alarm, int cmd, void *arg);
```

| 参数 | 描述 |
| ---- | ---- |
| alarm | 创建闹钟的 rt_alarm 结构体对象句柄 |
|cmd|选择控制 alarm 的模式，目前仅支持 RT_ALARM_CTRL_MODIFY 命令，修改闹钟模式和闹钟日期时间|
|arg|rt_alarm_setup 结构体对象地址|
|返回|——|
|成功|RT_EOK|
|失败|-RT_ERROR|

```c
注意：修改闹钟模式和日期时间后，alarm 闹钟并不会启动，需要调用 rt_alarm_start 函数启动 alarm 闹钟
```
#### 打印闹钟信息

通过如下函数打印 alarm 闹钟信息：

```c
void rt_alarm_dump(void);
```

#### Alarm 使用示例：

alarm 设备使用可以参考以下示例代码：

```c
/*
** 程序清单：这是一个 RTC 设备使用例程
** 例程导出了 alarm_sample 命令到控制终端
** 命令调用格式：alarm_sample
** 程序功能：设置RTC时间，创建闹钟，模式：每秒触发，启动闹钟
**/

void user_alarm_callback(rt_alarm_t alarm, time_t timestamp)
{
    rt_kprintf("user alarm callback function.\n");
}

void alarm_sample(void)
{  
    rt_device_t dev = rt_device_find("rtc");
    struct rt_alarm_setup setup;
    struct rt_alarm * alarm = RT_NULL;
    static time_t now;
    struct tm p_tm;
    
    if (alarm != RT_NULL)
        return;
    
    /* 获取当前时间戳，并把下一秒时间设置为闹钟时间 */
    now = time(NULL) + 1;
    gmtime_r(&now,&p_tm);
    
    setup.flag = RT_ALARM_SECOND;            
    setup.wktime.tm_year = p_tm.tm_year;
    setup.wktime.tm_mon = p_tm.tm_mon;
    setup.wktime.tm_mday = p_tm.tm_mday;
    setup.wktime.tm_wday = p_tm.tm_wday;
    setup.wktime.tm_hour = p_tm.tm_hour;
    setup.wktime.tm_min = p_tm.tm_min;
    setup.wktime.tm_sec = p_tm.tm_sec;   
    
    alarm = rt_alarm_create(user_alarm_callback, &setup);    
    if(RT_NULL != alarm)
    {
        rt_alarm_start(alarm);
    }
}
/* export msh cmd */
MSH_CMD_EXPORT(alarm_sample,alarm sample);
```

使用 finsh 执行代码大致效果如下 :
```c
msh >alarm_sample
user alarm callback function.
user alarm callback function.
user alarm callback function.
user alarm callback function.
user alarm callback function.

```

