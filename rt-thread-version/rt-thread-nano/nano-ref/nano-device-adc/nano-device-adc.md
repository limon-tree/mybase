# 在 RT-Thread Studio 上使用 RT-Thread Nano 并使用 ADC 设备接口

本文介绍了如何在 RT-Thread Studio 上使用 RT-Thread Nano，并基于 BearPI-IoT STM32L431RCT6 的基础工程进行讲解如何使用 ADC 设备接口。

## 为什么需要设备接口

1. RT-Thread 分为标准版本和 Nano 版本，其特点如下：
    - RT-Thread 标准版：拥有驱动框架，软件包等组件，软件包都是基于设备驱动接口来实现。
    - RT-Thread Nano ：仅仅只是一个 RTOS 内核。没有任何组件。
2. Nano 是无法直接使用 RT-Thread 丰富软件包功能。
3. Nano 是一个面向低资源的 MCU 等芯片，不可能增加如同标准版的设备驱动框架。
4. Nano 需要一套统一设备驱动 API，屏蔽不同芯片的 HAL 层的区别。方便移植工程到不同的平台。
4. Nano 需要一套设备驱动 API，可以方便使用丰富软件包组件。

## 准备工作

1. 使用 RT-Thread Studio 建立一个 STM32L431RCT6 的 RT-Thread Nano 基础工程。
2. 基础工程创建可参考：[在 RT-Thread Studio 上使用 RT-Thread Nano](../../nano-port-studio/an0047-nano-port-studio.md)

## ADC 设备接口

1. 在 RT-Thread 标准版中，[ADC 设备](../../../rt-thread-standard/programming-manual/device/adc/adc.md) 驱动提供了一套设备管理接口来访问 ADC，用户程序可以直接使用该 API 操作 ADC 的功能，设备管理接口如下：

|**函数**|**描述**    |
| --------------- | ------------------ |
| rt_device_find()  | 根据 ADC 设备名称查找设备获取设备句柄 |
| rt_adc_enable()     | 使能 ADC 设备     |
| rt_adc_read()   | 读取 ADC 设备数据      |
| rt_adc_disable()  | 关闭 ADC 设备    |

2. 由于 RT-Thread Nano 不使用设备驱动框架，所以没有对应的 rt_device_find() 这个 API 获取设备对象。当为了能够与 RT-Thread 标准版的接口相近，我们需要做了简单的修改，设备管理接口如下：

|**函数**|**描述**    |
| --------------- | ------------------ |
| rt_adc_device_find()  | 根据 ADC 设备名称查找设备获取设备句柄 |
| rt_adc_enable()     | 使能 ADC 设备     |
| rt_adc_read()   | 读取 ADC 设备数据      |
| rt_adc_disable()  | 关闭 ADC 设备    |

3. 对于 RT-Thread Nano，只需要适配如上这套 API，便可简单修改后使用 RT-Thread 丰富软件包功能。

## 适配 ADC 设备驱动接口

1. 复制 RT-Thread 完整版工程中的 adc.h 文件（路径：rt-thread\components\drivers\include\drivers\adc.h）到我们准备好的 STM32L431RCT6 的 RT-Thread Nano 基础工程中。

2. 由于 RT-Thread Nano 没有驱动框架，所以我们要把 adc.h 中有关完整版的内容去掉。整理完之后的 adc.h 文件如下：

```c
/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-08-22     RiceChen      first version
 */

#ifndef __ADC_H__
#define __ADC_H__
#include <rtthread.h>

struct rt_adc_device
{
    uint8_t *user_data;
};
typedef struct rt_adc_device *rt_adc_device_t;

typedef enum
{
    RT_ADC_CMD_ENABLE,
    RT_ADC_CMD_DISABLE,
} rt_adc_cmd_t;

struct rt_adc_device *rt_adc_device_find(const char *name);
rt_uint32_t rt_adc_read(rt_adc_device_t dev, rt_uint32_t channel);
rt_err_t rt_adc_enable(rt_adc_device_t dev, rt_uint32_t channel);
rt_err_t rt_adc_disable(rt_adc_device_t dev, rt_uint32_t channel);

#endif /* __ADC_H__ */
```

3. 我们需要适配如上 4 个 ADC 设备 API，参考实例：[drv_adc.c](./demo_code/drv_adc.c) 和 [drv_adc.h](./demo_code/drv_adc.h)。


## 编写 ADC 设备使用示例

```c
void adc_test(void)
{
    int32_t value = 0;
    struct rt_adc_device *dev = RT_NULL;
    dev = rt_adc_device_find("adc1");
    if(dev == RT_NULL)
    {
        rt_kprintf("%s not found\r\n", "adc1");
        return;
    }
    else
    {
        rt_adc_enable(dev, 3);
        value = rt_adc_read(dev, 3);
        rt_kprintf("adc value: %d\r\n", value);
    }
}
MSH_CMD_EXPORT(adc_test, adc test);
```

1. 实例代码运行现象：
```c
msh >adc_test
adc value: 565
msh >
```
