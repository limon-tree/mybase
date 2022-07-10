# 在 RT-Thread Studio 上使用 RT-Thread Nano 并使用 I2C 设备接口

本文介绍了如何在 RT-Thread Studio 上使用 RT-Thread Nano，并基于 BearPI-IoT STM32L431RCT6 的基础工程进行讲解如何使用 I2C 设备接口及相关软件包使用。

## 为什么需要设备接口

1. RT-Thread 分为标准版本和 Nano 版本，其特点如下：
    - RT-Thread 标准版：拥有设备驱动框架，软件包等组件，软件包都是基于设备驱动接口来实现。
    - RT-Thread Nano：仅仅只是一个 RTOS 内核。没有任何组件。
2. Nano 是无法直接使用 RT-Thread 丰富软件包功能。
3. Nano 是一个面向低资源的 MCU 等芯片，不可能增加如同标准版的设备驱动框架。
4. Nano 需要一套统一设备驱动 API，屏蔽不同芯片的 HAL 层的区别。方便移植工程到不同的平台。
4. Nano 需要一套设备驱动 API，可以方便使用丰富软件包组件。

## 准备工作

1. 使用 RT-Thread Studio 建立一个 STM32L431RCT6 的 RT-Thread Nano 基础工程。
2. 基础工程创建可参考：[在 RT-Thread Studio 上使用 RT-Thread Nano](../../nano-port-studio/an0047-nano-port-studio.md)

## I2C 设备接口

1. 在 RT-Thread 标准版中，[I2C 设备](../../../rt-thread-standard/programming-manual/device/i2c/i2c.md) 驱动提供了一套设备管理接口来访问 I2C，用户程序可以直接使用该 API 操作 I2C 的功能，设备管理接口如下：

|**函数**|**描述**                          |
| --------------- | ---------------------------------- |
| rt_device_find()  | 根据 I2C 总线设备名称查找设备获取设备句柄      |
| rt_i2c_transfer() | 传输数据     |

2. 由于 RT-Thread Nano 不使用设备驱动框架，所以没有对应的 rt_device_find() 这个 API 获取设备对象。但 RT-Thread 标准版实际为用户层提供了另外一套 API 给用户层使用。设备管理接口如下：

|**函数**|**描述**                          |
| --------------- | ---------------------------------- |
| rt_i2c_bus_device_find()  | 根据 I2C 总线设备名称查找设备获取设备句柄      |
| rt_i2c_transfer() | 传输数据     |
| rt_i2c_master_send() | 发送数据     |
| rt_i2c_master_recv() | 接收数据     |

3. 对于 RT-Thread Nano，只需要适配如上这套 API，便可简单修改后使用 RT-Thread 丰富软件包功能。

## 适配 I2C 设备接口

1. 复制 RT-Thread 完整版工程中的 i2c.h 文件（路径：rt-thread\components\drivers\include\drivers\i2c.h）到我们准备好的 STM32L431RCT6 的 RT-Thread Nano 基础工程中。

2. 由于 RT-Thread Nano 没有设备驱动框架，所以我们要把 i2c.h 中有关完整版的内容去掉。整理完之后的 i2c.h 文件如下：

```c
/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author        Notes
 * 2021-04-20     RiceChen      first version
 */

#ifndef __I2C_H__
#define __I2C_H__

#include <rtthread.h>

#ifdef __cplusplus
extern "C" {
#endif

#define RT_I2C_WR                0x0000
#define RT_I2C_RD               (1u << 0)
#define RT_I2C_ADDR_10BIT       (1u << 2)  /* this is a ten bit chip address */
#define RT_I2C_NO_START         (1u << 4)
#define RT_I2C_IGNORE_NACK      (1u << 5)
#define RT_I2C_NO_READ_ACK      (1u << 6)  /* when I2C reading, we do not ACK */
#define RT_I2C_NO_STOP          (1u << 7)

struct rt_i2c_config
{
    char *name;
    rt_uint8_t scl;
    rt_uint8_t sda;
};

struct rt_i2c_msg
{
    rt_uint16_t addr;
    rt_uint16_t flags;
    rt_uint16_t len;
    rt_uint8_t  *buf;
};

/*for i2c bus driver*/
struct rt_i2c_bus_device
{
    struct rt_i2c_config *config;
    rt_uint16_t  flags;
    rt_uint16_t  addr;
    struct rt_mutex lock;
    rt_uint32_t  timeout;
    rt_uint32_t  retries;
    void *priv;
};

struct rt_i2c_bus_device *rt_i2c_bus_device_find(const char *bus_name);
rt_size_t rt_i2c_transfer(struct rt_i2c_bus_device *bus,
                          struct rt_i2c_msg         msgs[],
                          rt_uint32_t               num);
rt_err_t rt_i2c_control(struct rt_i2c_bus_device *bus,
                        rt_uint32_t               cmd,
                        rt_uint32_t               arg);
rt_size_t rt_i2c_master_send(struct rt_i2c_bus_device *bus,
                             rt_uint16_t               addr,
                             rt_uint16_t               flags,
                             const rt_uint8_t         *buf,
                             rt_uint32_t               count);
rt_size_t rt_i2c_master_recv(struct rt_i2c_bus_device *bus,
                             rt_uint16_t               addr,
                             rt_uint16_t               flags,
                             rt_uint8_t               *buf,
                             rt_uint32_t               count);

rt_inline rt_err_t rt_i2c_bus_lock(struct rt_i2c_bus_device *bus, rt_tick_t timeout)
{
    return rt_mutex_take(&bus->lock, timeout);
}

rt_inline rt_err_t rt_i2c_bus_unlock(struct rt_i2c_bus_device *bus)
{
    return rt_mutex_release(&bus->lock);
}

int rt_i2c_core_init(void);

#ifdef __cplusplus
}
#endif

#endif

```

3. 我们需要适配如上 6 个 I2C 设备 API ，参考实例：[drv_i2c.c](./demo_code/drv_i2c.c) 和 [drv_i2c.h](./demo_code/drv_i2c.h)。

|**函数**|**描述**                          |
| --------------- | ---------------------------------- |
| rt_i2c_core_init()  | I2C 总线初始化      |
| rt_i2c_bus_device_find() | 根据 I2C 总线设备名称查找设备获取设备句柄     |
| rt_i2c_transfer() | 数据传输     |
| rt_i2c_control() | I2C 总线配置     |
| rt_i2c_master_send() | 发送数据     |
| rt_i2c_master_recv() | 接收数据     |


## 编写 I2C 设备使用示例

```c
#include <rtthread.h>
#include <rtdevice.h>

#define AHT10_I2C_BUS_NAME          "i2c1"  /* 传感器连接的 I2C 总线设备名称 */
#define AHT10_ADDR                  0x38    /* 从机地址 */
#define AHT10_CALIBRATION_CMD       0xE1    /* 校准命令 */
#define AHT10_NORMAL_CMD            0xA8    /* 一般命令 */
#define AHT10_GET_DATA              0xAC    /* 获取数据命令 */

static struct rt_i2c_bus_device *i2c_bus = RT_NULL;     /* I2C 总线设备句柄 */
static rt_bool_t initialized = RT_FALSE;                /* 传感器初始化状态 */

/* 写传感器寄存器 */
static rt_err_t write_reg(struct rt_i2c_bus_device *bus, rt_uint8_t reg, rt_uint8_t *data)
{
    rt_uint8_t buf[3];
    struct rt_i2c_msg msgs;
    rt_uint32_t buf_size = 1;

    buf[0] = reg; //cmd
    if (data != RT_NULL)
    {
        buf[1] = data[0];
        buf[2] = data[1];
        buf_size = 3;
    }

    msgs.addr = AHT10_ADDR;
    msgs.flags = RT_I2C_WR;
    msgs.buf = buf;
    msgs.len = buf_size;

    /* 调用 I2C 设备接口传输数据 */
    if (rt_i2c_transfer(bus, &msgs, 1) == 1)
    {
        return RT_EOK;
    }
    else
    {
        return -RT_ERROR;
    }
}

/* 读传感器寄存器数据 */
static rt_err_t read_regs(struct rt_i2c_bus_device *bus, rt_uint8_t len, rt_uint8_t *buf)
{
    struct rt_i2c_msg msgs;

    msgs.addr = AHT10_ADDR;
    msgs.flags = RT_I2C_RD;
    msgs.buf = buf;
    msgs.len = len;

    /* 调用 I2C 设备接口传输数据 */
    if (rt_i2c_transfer(bus, &msgs, 1) == 1)
    {
        return RT_EOK;
    }
    else
    {
        return -RT_ERROR;
    }
}

static void read_temp_humi(float *cur_temp, float *cur_humi)
{
    rt_uint8_t temp[6];

    write_reg(i2c_bus, AHT10_GET_DATA, RT_NULL);      /* 发送命令 */
    rt_thread_mdelay(400);
    read_regs(i2c_bus, 6, temp);                /* 获取传感器数据 */

    /* 湿度数据转换 */
    *cur_humi = (temp[1] << 12 | temp[2] << 4 | (temp[3] & 0xf0) >> 4) * 100.0 / (1 << 20);
    /* 温度数据转换 */
    *cur_temp = ((temp[3] & 0xf) << 16 | temp[4] << 8 | temp[5]) * 200.0 / (1 << 20) - 50;
}

static void aht10_init(const char *name)
{
    rt_uint8_t temp[2] = {0, 0};

    /* 查找 I2C 总线设备，获取 I2C 总线设备句柄 */
    i2c_bus = rt_i2c_bus_device_find(name);

    if (i2c_bus == RT_NULL)
    {
        rt_kprintf("can't find %s device!\n", name);
    }
    else
    {
        write_reg(i2c_bus, AHT10_NORMAL_CMD, temp);
        rt_thread_mdelay(400);

        temp[0] = 0x08;
        temp[1] = 0x00;
        write_reg(i2c_bus, AHT10_CALIBRATION_CMD, temp);
        rt_thread_mdelay(400);
        initialized = RT_TRUE;
    }
}

static void i2c_aht10_sample(int argc, char *argv[])
{
    float humidity, temperature;
    char name[RT_NAME_MAX];

    humidity = 0.0;
    temperature = 0.0;

    if (argc == 2)
    {
        rt_strncpy(name, argv[1], RT_NAME_MAX);
    }
    else
    {
        rt_strncpy(name, AHT10_I2C_BUS_NAME, RT_NAME_MAX);
    }

    if (!initialized)
    {
        /* 传感器初始化 */
        aht10_init(name);
    }
    if (initialized)
    {
        /* 读取温湿度数据 */
        read_temp_humi(&temperature, &humidity);

        rt_kprintf("read aht10 sensor humidity   : %d.%d %%\n", (int)humidity, (int)(humidity * 10) % 10);
        if(temperature>= 0 )
        {
            rt_kprintf("read aht10 sensor temperature: %d.%d°C\n", (int)temperature, (int)(temperature * 10) % 10);
        }
        else
        {
            rt_kprintf("read aht10 sensor temperature: %d.%d°C\n", (int)temperature, (int)(-temperature * 10) % 10);
        }
    }
    else
    {
        rt_kprintf("initialize sensor failed!\n");
    }
}
/* 导出到 msh 命令列表中 */
MSH_CMD_EXPORT(i2c_aht10_sample, i2c aht10 sample);

实例代码运行现象：

```c
msh >i2c_aht10_sample i2c1
read aht10 sensor humidity   : 90.0 %
read aht10 sensor temperature: 25.33°C
msh >
```


## I2C 设备相关软件包使用

1. 我们使用 [as7341 软件包](http://packages.rt-thread.org/detail.html?package=as7341) 来验证 I2C 设备 API。

2. 首先克隆 as7341 软件包到 STM32L431RCT6 的 RT-Thread Nano 工程。as7341 软件包链接：https://github.com/RiceChen/as7341.git

3. 由于没有了 RT-Thread 标准版本的设备驱动框架，所以对软件包进行简单的修改：
    - 在 as7341.h 中包含 drv_i2c.h 头文件。
    - 修改 as7341.c 中的测试用例。代码如下：

```c
static void as7341(int argc, char *argv[])
{
    static as7341_device_t dev = RT_NULL;

    if (argc> 1)
    {
        if (!strcmp(argv[1], "probe"))
        {
            if (argc>= 3)
            {
                /* initialize the sensor when first probe */
                if (!dev || strcmp(dev->i2c->config->name, argv[2])) // 修改点 1
                {
                    /* deinit the old device */
                    if(dev)
                    {
						rt_kprintf("Deinit as7341\n");
                        as7341_deinit(dev);
                    }

                    dev = as7341_init(argv[2], eSpm);
                    if(!dev)
                    {
                        rt_kprintf("as7341 probe failed, check input args\n");
                    }else
					{
						rt_kprintf("as7341 probed, addr:0x%x\n", AS7341_ADDR) ;
					}
                }
            }
            else
            {
                as7341_usage();
            }
        }
        else if (!strcmp(argv[1], "read"))
        {
            if (dev)
            {
                if(!strcmp(argv[2], "spectral"))
                {
                    MODE_ONE_DATA_t data1;
                    MODE_TOW_DATA_t data2;

                    as7341_start_measure(dev, eF1F4ClearNIR);
                    data1 = as7341_read_spectral_data_one(dev);
                    rt_kprintf("F1(405-425nm): %d\n", data1.ADF1);
                    rt_kprintf("F2(435-455nm): %d\n", data1.ADF2);
                    rt_kprintf("F3(470-490nm): %d\n", data1.ADF3);
                    rt_kprintf("F4(505-525nm): %d\n", data1.ADF4);

                    as7341_start_measure(dev, eF5F8ClearNIR);
                    data2 = as7341_read_spectral_data_tow(dev);
                    rt_kprintf("F5(545-565nm): %d\n", data2.ADF5);
                    rt_kprintf("F6(580-600nm): %d\n", data2.ADF6);
                    rt_kprintf("F7(620-640nm): %d\n", data2.ADF7);
                    rt_kprintf("F8(670-690nm): %d\n", data2.ADF8);

                    rt_kprintf("Clear: %d\n", data2.ADCLEAR);
                    rt_kprintf("NIR: %d\n", data2.ADNIR);
                }
                else if(!strcmp(argv[2], "flicker"))
                {
                    rt_uint8_t freq = 0;
                    freq = as7341_read_flicker_data(dev);
                    if(freq == 1)
                    {
                        rt_kprintf("Unknown frequency\n");
                    }
                    else if(freq == 0)
                    {
                        rt_kprintf("No flicker\n");
                    }
                    else
                    {
                        rt_kprintf("freq: %dHz\n", freq);
                    }
                }
                else
                {
                    as7341_usage();
                }

            }
            else
            {
                rt_kprintf("Please using'as7341 probe <i2c dev name>'first\n");
            }
        }
        else
        {
            as7341_usage();
        }
    }
    else
    {
        as7341_usage();
    }
}
```

3. 使用 as7341 软件包实例，编译烧录便可以在终端输入测试命令：

```c
msh >as7341 probe i2c1
as7341 id: 0x24
as7341 probed, addr:0x39
msh >
msh >as7341 read spectral
F1(405-425nm): 1
F2(435-455nm): 3
F3(470-490nm): 4
F4(505-525nm): 5
F5(545-565nm): 7
F6(580-600nm): 6
F7(620-640nm): 7
F8(670-690nm): 4
Clear: 22
NIR: 2
msh >
```


