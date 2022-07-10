# SENSOR 设备

## SENSOR 简介

Sensor（传感器）是物联网重要的一部分，“Sensor 之于物联网”就相当于“眼睛之于人类”。人类如果没有了眼睛就看不到这大千的花花世界，对于物联网来说也是一样。

如今随着物联网的发展，已经有大量的 Sensor 被开发出来供开发者选择了，如：加速度计(Accelerometer)、磁力计(Magnetometer)、陀螺仪(Gyroscope)、气压计(Barometer/pressure)、湿度计(Humidometer)等。这些传感器，世界上的各大半导体厂商都有生产，虽然增加了市场的可选择性，同时也加大了应用程序开发的难度。因为不同的传感器厂商、不同的传感器都需要配套自己独有的驱动才能运转起来，这样在开发应用程序的时候就需要针对不同的传感器做适配，自然加大了开发难度。为了降低应用开发的难度，增加传感器驱动的可复用性，我们设计了 Sensor 设备。

Sensor 设备的作用是：为上层提供统一的操作接口，提高上层代码的可重用性。

### 传感器设备特性

- 接口：标准 device 接口(open/close/read/control)
- 工作模式：支持 轮询、中断、FIFO 三种模式
- 电源模式：支持 掉电、普通、低功耗、高功耗 四种模式

点击[传感器列表](sensor_list.md)，查看当前支持的传感器

## 访问传感器设备

应用程序通过 RT-Thread 提供的 I/O 设备管理接口来访问传感器设备，相关接口如下所示：

| **函数**                    | **描述**                                   |
| --------------------------- | ------------------------------------------ |
| rt_device_find()            | 根据传感器设备设备名称查找设备获取设备句柄 |
| rt_device_open()            | 打开传感器设备                             |
| rt_device_read()            | 读取数据                                   |
| rt_device_control()         | 控制传感器设备                             |
| rt_device_set_rx_indicate() | 设置接收回调函数                           |
| rt_device_close()           | 关闭传感器设备                             |

### 查找传感器

应用程序根据传感器设备名称获取设备句柄，进而可以操作传感器设备，查找设备函数如下所示：

```c
rt_device_t rt_device_find(const char* name);
```

| **参数** | **描述**                           |
| -------- | ---------------------------------- |
| name     | 传感器设备名称                           |
| **返回** | ——                                 |
| 设备句柄 | 查找到对应设备将返回相应的设备句柄 |
| RT_NULL  | 没有找到相应的设备对象             |

使用示例如下所示：
```c
#define SENSOR_DEVICE_NAME    "acce_st"    /* 传感器设备名称 */

static rt_device_t sensor_dev;         /* 传感器设备句柄 */
/* 根据设备名称查找传感器设备，获取设备句柄 */
sensor_dev = rt_device_find(SENSOR_DEVICE_NAME);
```

### 打开传感器设备

通过设备句柄，应用程序可以打开和关闭设备，打开设备时，会检测设备是否已经初始化，没有初始化则会默认调用初始化接口初始化设备。通过如下函数打开设备:

```c
rt_err_t rt_device_open(rt_device_t dev, rt_uint16_t oflags);
```

| **参数**   | **描述**                         |
| ---------- | ------------------------------- |
| dev        | 设备句柄                        |
| oflags     | 设备模式标志                     |
| **返回**   | ——                             |
| RT_EOK     | 设备打开成功                |
| -RT_EBUSY  | 如果设备注册时指定的参数中包括 RT_DEVICE_FLAG_STANDALONE 参数，此设备将不允许重复打开 |
| -RT_EINVAL  | 不支持的打开参数 |
| 其他错误码 | 设备打开失败                |

oflags 参数支持下列参数：

```c
#define RT_DEVICE_FLAG_RDONLY       0x001     /* 标准设备的只读模式，对应传感器的轮询模式 */
#define RT_DEVICE_FLAG_INT_RX       0x100     /* 中断接收模式 */
#define RT_DEVICE_FLAG_FIFO_RX      0x200     /* FIFO 接收模式 */
```

传感器数据接收和发送数据的模式分为 3 种：中断模式、轮询模式、FIFO 模式。在使用的时候，这 3 种模式只能**选其一**，若传感器的打开参数 oflags 没有指定使用中断模式或者 FIFO 模式，则默认使用轮询模式。

FIFO（First Input First Output）即先进先出。 FIFO 传输方式需要传感器硬件支持，数据存储在硬件 FIFO 中，一次读取可以读取多个数据，这就节省了 CPU 的资源来做其他操作。在低功耗模式时非常有用。

若传感器要使用 FIFO 接收模式，oflags 取值 RT_DEVICE_FLAG_FIFO_RX。

以轮询模式打开传感器设备使用示例如下所示：

```c
#define SAMPLE_SENSOR_NAME       "acce_st"  /* 传感器设备名称 */
int main(void)
{
    rt_device_t dev;
    struct rt_sensor_data data;

    /* 查找传感器设备 */
    dev = rt_device_find(SAMPLE_SENSOR_NAME);
    /* 以只读及轮询模式打开传感器设备 */
    rt_device_open(dev, RT_DEVICE_FLAG_RDONLY);

    if (rt_device_read(dev, 0, &data, 1) == 1)
    {
        rt_kprintf("acce: x:%5d, y:%5d, z:%5d, timestamp:%5d\n", data.data.acce.x, data.data.acce.y, data.data.acce.z, data.timestamp);
    }
    rt_device_close(dev);

    return RT_EOK;
}
```
### 控制传感器设备

通过命令控制字，应用程序可以对传感器设备进行配置，通过如下函数完成：

```c
rt_err_t rt_device_control(rt_device_t dev, rt_uint8_t cmd, void* arg);
```

| **参数**   | **描述**                                   |
| ---------- | ------------------------------------------ |
| dev        | 设备句柄                                   |
| cmd        | 命令控制字，详细介绍见下面 |
| arg        | 控制的参数, 详细介绍见下面  |
| **返回**   | ——                                         |
| RT_EOK     | 函数执行成功                               |
| -RT_ENOSYS | 执行失败，dev 为空                         |
| 其他错误码 | 执行失败                                   |

其中的 cmd 目前支持以下几种命令控制字

```c
#define  RT_SENSOR_CTRL_GET_ID        /* 读设备ID */
#define  RT_SENSOR_CTRL_GET_INFO      /* 获取设备信息 */
#define  RT_SENSOR_CTRL_SET_RANGE     /* 设置传感器测量范围 */
#define  RT_SENSOR_CTRL_SET_ODR       /* 设置传感器数据输出速率，unit is HZ */
#define  RT_SENSOR_CTRL_SET_POWER     /* 设置电源模式 */
#define  RT_SENSOR_CTRL_SELF_TEST     /* 自检 */
```
#### 获取设备信息

```c
struct rt_sensor_info info;
rt_device_control(dev, RT_SENSOR_CTRL_GET_INFO, &info);
LOG_I("vendor :%d", info.vendor);
LOG_I("model  :%s", info.model);
LOG_I("unit   :%d", info.unit);
LOG_I("intf_type :%d", info.intf_type);
LOG_I("period_min:%d", info.period_min);
```

#### 读设备ID

```c
rt_uint8_t reg = 0xFF;
rt_device_control(dev, RT_SENSOR_CTRL_GET_ID, &reg);
LOG_I("device id: 0x%x!", reg);
```

#### 设置测量范围

设置传感器测量范围时的单位为设备注册时提供的单位。

```c
rt_device_control(dev, RT_SENSOR_CTRL_SET_RANGE, (void *)1000);
```

#### 设置数据输出速率

设置输出速率为 100HZ，调用下面的接口。

```c
rt_device_control(dev, RT_SENSOR_CTRL_SET_ODR, (void *)100);
```

####  设置电源模式

```c
/* 设置电源模式为掉电模式 */
rt_device_control(dev, RT_SENSOR_CTRL_SET_POWER, (void *)RT_SEN_POWER_DOWN);
/* 设置电源模式为普通模式 */
rt_device_control(dev, RT_SENSOR_CTRL_SET_POWER, (void *)RT_SEN_POWER_NORMAL);
/* 设置电源模式为低功耗模式 */
rt_device_control(dev, RT_SENSOR_CTRL_SET_POWER, (void *)RT_SEN_POWER_LOW);
/* 设置电源模式为高性能模式 */
rt_device_control(dev, RT_SENSOR_CTRL_SET_POWER, (void *)RT_SEN_POWER_HIGH);
```

####  设备自检

```c
int test_res;
/* 控制设备自检 并把结果返回回来，RT_EOK 自检成功, 其他自检失败 */
rt_device_control(dev, RT_SENSOR_CTRL_SELF_TEST, &test_res);
```

### 设置接收回调函数

可以通过如下函数来设置数据接收指示，当传感器收到数据时，通知上层应用线程有数据到达 ：

```c
rt_err_t rt_device_set_rx_indicate(rt_device_t dev, rt_err_t (*rx_ind)(rt_device_t dev,rt_size_t size));
```

| **参数** | **描述**     |
| -------- | ------------ |
| dev      | 设备句柄     |
| rx_ind   | 回调函数指针 |
| dev      | 设备句柄（回调函数参数）|
| size     | 缓冲区数据大小（回调函数参数）|
| **返回** | ——           |
| RT_EOK   | 设置成功     |

该函数的回调函数由调用者提供。若传感器以中断接收模式打开，当传感器接收到数据产生中断时，就会调用回调函数，并且会把此时缓冲区的数据大小放在 size 参数里，把传感器设备句柄放在 dev 参数里供调用者获取。

一般情况下接收回调函数可以发送一个信号量或者事件通知传感器数据处理线程有数据到达。使用示例如下所示：

```c
#define SAMPLE_SENSOR_NAME       "acce_st"  /* 传感器设备名称 */
static rt_device_t dev;             /* 传感器设备句柄 */
static struct rt_semaphore rx_sem;    /* 用于接收消息的信号量 */

/* 接收数据回调函数 */
static rt_err_t sensor_input(rt_device_t dev, rt_size_t size)
{
    /* 传感器接收到数据后产生中断，调用此回调函数，然后发送接收信号量 */
    rt_sem_release(&rx_sem);

    return RT_EOK;
}

static int sensor_sample(int argc, char *argv[])
{
    dev = rt_device_find(SAMPLE_SENSOR_NAME);

    /* 以中断接收及轮询发送模式打开传感器设备 */
    rt_device_open(dev, RT_DEVICE_FLAG_INT_RX);
    /* 初始化信号量 */
    rt_sem_init(&rx_sem, "rx_sem", 0, RT_IPC_FLAG_PRIO);

    /* 设置接收回调函数 */
    rt_device_set_rx_indicate(dev, sensor_input);
}

```

### 读取数据

可调用如下函数读取传感器接收到的数据：

```c
rt_size_t rt_device_read(rt_device_t dev, rt_off_t pos, void* buffer, rt_size_t size);
```

| **参数**           | **描述**                                       |
| ------------------ | ---------------------------------------------- |
| dev                | 设备句柄                                       |
| pos                | 读取数据偏移量，此参数传感器未使用                 |
| buffer             | 缓冲区指针，读取的数据将会被保存在缓冲区中        |
| size               | 读取数据的个数                                 |
| **返回**           | ——                                             |
| >0                 | 返回读取到数据的个数                            |
| 0                  | 需要读取当前线程的 errno 来判断错误状态          |

传感器使用中断接收模式并配合接收回调函数的使用示例如下所示：

```c
static rt_device_t dev;             /* 传感器设备句柄 */
static struct rt_semaphore rx_sem;    /* 用于接收消息的信号量 */

/* 接收数据的线程 */
static void sensor_irq_rx_entry(void *parameter)
{
    rt_device_t dev = parameter;
    struct rt_sensor_data data;
    rt_size_t res;

    while (1)
    {
        rt_sem_take(rx_sem, RT_WAITING_FOREVER);

        res = rt_device_read(dev, 0, &data, 1);
        if (res == 1)
        {
            sensor_show_data(dev, &data);
        }
    }
}

```

传感器使用 FIFO 接收模式并配合接收回调函数的使用示例如下所示：

```c
static rt_sem_t sensor_rx_sem = RT_NULL;
rt_err_t rx_cb(rt_device_t dev, rt_size_t size)
{
    rt_sem_release(sensor_rx_sem);
    return 0;
}
static void sensor_fifo_rx_entry(void *parameter)
{
    rt_device_t dev = parameter;
    struct rt_sensor_data data;
    rt_size_t res, i;

    data = rt_malloc(sizeof(struct rt_sensor_data) * 32);

    while (1)
    {
        rt_sem_take(sensor_rx_sem, RT_WAITING_FOREVER);

        res = rt_device_read(dev, 0, data, 32);
        for (i = 0; i < res; i++)
        {
            sensor_show_data(dev, &data[i]);
        }
    }
}
int main(void)
{
    static rt_thread_t tid1 = RT_NULL;
    rt_device_t dev;
    struct rt_sensor_data data;

    sensor_rx_sem = rt_sem_create("sen_rx_sem", 0, RT_IPC_FLAG_PRIO);
    tid1 = rt_thread_create("sen_rx_thread",
                            sensor_fifo_rx_entry, dev,
                            1024,
                            15, 5);
    if (tid1 != RT_NULL)
        rt_thread_startup(tid1);

    dev = rt_device_find("acce_st");
    rt_device_set_rx_indicate(dev, rx_cb);
    rt_device_open(dev, RT_SEN_FLAG_FIFO);
    return RT_EOK;
}
```

### 关闭传感器设备

当应用程序完成传感器操作后，可以关闭传感器设备，通过如下函数完成:

```c
rt_err_t rt_device_close(rt_device_t dev);
```

| **参数**   | **描述**                           |
| ---------- | ---------------------------------- |
| dev        | 设备句柄                           |
| **返回**   | ——                                 |
| RT_EOK     | 关闭设备成功                       |
| -RT_ERROR  | 设备已经完全关闭，不能重复关闭设备 |
| 其他错误码 | 关闭设备失败                       |

关闭设备接口和打开设备接口需配对使用，打开一次设备对应要关闭一次设备，这样设备才会被完全关闭，否则设备仍处于未关闭状态。

## 传感器设备使用示例

传感器设备的具体使用方式可以参考如下示例代码，示例代码的主要步骤如下：

1. 首先查找传感器设置获取设备句柄。

2. 以轮询的方式打开传感器。

3. 连续读取 5 次数据并打印出来。

4. 关闭传感器。

* 此示例代码不局限于特定的 BSP，根据 BSP 注册的传感器设备，输入不同的 dev_name 即可运行。

```c
/*
 * 程序清单：这是一个 传感器 设备使用例程
 * 例程导出了 sensor_sample 命令到控制终端
 * 命令调用格式：sensor_sample dev_name
 * 命令解释：命令第二个参数是要使用的传感器设备名称
 * 程序功能：打开对应的传感器，然后连续读取 5 次数据并打印出来。
*/

#include "sensor.h"

static void sensor_show_data(rt_size_t num, rt_sensor_t sensor, struct rt_sensor_data *sensor_data)
{
    switch (sensor->info.type)
    {
    case RT_SENSOR_CLASS_ACCE:
        rt_kprintf("num:%3d, x:%5d, y:%5d, z:%5d, timestamp:%5d\n", num, sensor_data->data.acce.x, sensor_data->data.acce.y, sensor_data->data.acce.z, sensor_data->timestamp);
        break;
    case RT_SENSOR_CLASS_GYRO:
        rt_kprintf("num:%3d, x:%8d, y:%8d, z:%8d, timestamp:%5d\n", num, sensor_data->data.gyro.x, sensor_data->data.gyro.y, sensor_data->data.gyro.z, sensor_data->timestamp);
        break;
    case RT_SENSOR_CLASS_MAG:
        rt_kprintf("num:%3d, x:%5d, y:%5d, z:%5d, timestamp:%5d\n", num, sensor_data->data.mag.x, sensor_data->data.mag.y, sensor_data->data.mag.z, sensor_data->timestamp);
        break;
    case RT_SENSOR_CLASS_HUMI:
        rt_kprintf("num:%3d, humi:%3d.%d%%, timestamp:%5d\n", num, sensor_data->data.humi / 10, sensor_data->data.humi % 10, sensor_data->timestamp);
        break;
    case RT_SENSOR_CLASS_TEMP:
        rt_kprintf("num:%3d, temp:%3d.%dC, timestamp:%5d\n", num, sensor_data->data.temp / 10, sensor_data->data.temp % 10, sensor_data->timestamp);
        break;
    case RT_SENSOR_CLASS_BARO:
        rt_kprintf("num:%3d, press:%5d, timestamp:%5d\n", num, sensor_data->data.baro, sensor_data->timestamp);
        break;
    case RT_SENSOR_CLASS_STEP:
        rt_kprintf("num:%3d, step:%5d, timestamp:%5d\n", num, sensor_data->data.step, sensor_data->timestamp);
        break;
    default:
        break;
    }
}

static void sensor_sample(int argc, char **argv)
{
    rt_device_t dev = RT_NULL;
    struct rt_sensor_data data;
    rt_size_t res, i;

    /* 查找系统中的传感器设备 */
    dev = rt_device_find(argv[1]);
    if (dev == RT_NULL)
    {
        rt_kprintf("Can't find device:%s\n", argv[1]);
        return;
    }

    /* 以轮询模式打开传感器设备 */
    if (rt_device_open(dev, RT_DEVICE_FLAG_RDONLY) != RT_EOK)
    {
        rt_kprintf("open device failed!");
        return;
    }

    for (i = 0; i < 5; i++)
    {
        /* 从传感器读取一个数据 */
        res = rt_device_read(dev, 0, &data, 1);
        if (res != 1)
        {
            rt_kprintf("read data failed!size is %d", res);
        }
        else
        {
            sensor_show_data(i, (rt_sensor_t)dev, &data);
        }
        rt_thread_mdelay(100);
    }
    /* 关闭传感器设备 */
    rt_device_close(dev);
}
MSH_CMD_EXPORT(sensor_sample, sensor device sample);
```

