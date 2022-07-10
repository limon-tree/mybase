# 在 RT-Thread Studio 上使用 RT-Thread Nano 并使用 PIN 设备接口

本文介绍了如何在 RT-Thread Studio 上使用 RT-Thread Nano，并基于 BearPI-IoT STM32L431RCT6 的基础工程进行讲解如何使用 PIN 设备接口及相关软件包使用。

## 为什么需要设备接口

1. RT-Thread 分为标准版本和 Nano 版本，其特点如下：
    - RT-Thread 标准版：拥有设备驱动框架，软件包等组件，软件包都是基于设备驱动接口来实现。
    - RT-Thread Nano：仅仅只是一个 RTOS 内核。没有任何组件。
2. Nano 是无法直接使用 RT-Thread 丰富软件包功能。
3. Nano 是一个面向低资源的 MCU 等芯片，不可能增加如同标准版的设备驱动框架。
4. Nano 需要一套统一设备驱动 API ，屏蔽不同芯片的 HAL 层的区别。方便移植工程到不同的平台。
4. Nano 需要一套设备驱动 API ，可以方便使用丰富软件包组件。

## 准备工作

1. 使用 RT-Thread Studio 建立一个 STM32L431RCT6 的 RT-Thread Nano 基础工程。
2. 基础工程创建可参考：[在 RT-Thread Studio 上使用 RT-Thread Nano](../../nano-port-studio/an0047-nano-port-studio.md)

## PIN 设备接口

1. 在 RT-Thread 标准版中，[PIN 设备](../../../rt-thread-standard/programming-manual/device/pin/pin.md) 设备提供了一套设备管理接口来访问 GPIO，用户程序可以直接使用该 API 操作 GPIO 的功能，设备管理接口如下：

|**函数**|**描述**                          |
| ---------------- | ---------------------- |
| rt_pin_get()  | 获取引脚编号  |
| rt_pin_mode()  | 设置引脚模式  |
| rt_pin_write()     | 设置引脚电平     |
| rt_pin_read()   | 读取引脚电平      |
| rt_pin_attach_irq()  | 绑定引脚中断回调函数  |
| rt_pin_irq_enable()   | 使能引脚中断      |
| rt_pin_detach_irq()  | 脱离引脚中断回调函数  |

2. RT-Thread 丰富软件包都是基于这套 API 进行开发适配，所以 Nano 也需要一套这样子的 API。在 PIN 设备接口，可以完全沿用标准版的这套 API。

## 适配 PIN 设备接口

1. 复制 RT-Thread 完整版工程中的 pin.h 文件（路径：rt-thread\components\drivers\include\drivers\pin.h）到我们准备好的 STM32L431RCT6 的 RT-Thread Nano 基础工程中。

2. 由于 RT-Thread Nano 没有驱动框架，所以我们要把 pin.h 中有关完整版的内容去掉。整理完之后的 pin.h 文件如下：

```c
/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-08-21     RiceChen     the first version
 */

#ifndef PIN_H__
#define PIN_H__

#include <rtthread.h>

#ifdef __cplusplus
extern "C" {
#endif

#define PIN_LOW                 0x00
#define PIN_HIGH                0x01

#define PIN_MODE_OUTPUT         0x00
#define PIN_MODE_INPUT          0x01
#define PIN_MODE_INPUT_PULLUP   0x02
#define PIN_MODE_INPUT_PULLDOWN 0x03
#define PIN_MODE_OUTPUT_OD      0x04

#define PIN_IRQ_MODE_RISING             0x00
#define PIN_IRQ_MODE_FALLING            0x01
#define PIN_IRQ_MODE_RISING_FALLING     0x02
#define PIN_IRQ_MODE_HIGH_LEVEL         0x03
#define PIN_IRQ_MODE_LOW_LEVEL          0x04

#define PIN_IRQ_DISABLE                 0x00
#define PIN_IRQ_ENABLE                  0x01

#define PIN_IRQ_PIN_NONE                -1

struct rt_device_pin_mode
{
    rt_uint16_t pin;
    rt_uint16_t mode;
};
struct rt_device_pin_status
{
    rt_uint16_t pin;
    rt_uint16_t status;
};
struct rt_pin_irq_hdr
{
    rt_int16_t        pin;
    rt_uint16_t       mode;
    void (*hdr)(void *args);
    void             *args;
};

void rt_pin_mode(rt_base_t pin, rt_base_t mode);
void rt_pin_write(rt_base_t pin, rt_base_t value);
int  rt_pin_read(rt_base_t pin);
rt_err_t rt_pin_attach_irq(rt_int32_t pin, rt_uint32_t mode,
                             void (*hdr)(void *args), void  *args);
rt_err_t rt_pin_detach_irq(rt_int32_t pin);
rt_err_t rt_pin_irq_enable(rt_base_t pin, rt_uint32_t enabled);
/* Get pin number by name,such as PA.0,P0.12 */
rt_base_t rt_pin_get(const char *name);

#ifdef __cplusplus
}
#endif

#endif
```

3. 我们需要适配如上 7 个 PIN 设备 API ，参考实例：[drv_gpio.c](./demo_code/drv_gpio.c) 和 [drv_gpio.h](./demo_code/drv_gpio.h)。

## 编写 PIN 设备使用示例

```c
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-08-21     RT-Thread    first version
 */

#include <rtthread.h>

#define DBG_TAG "main"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

#include "drv_gpio.h"
#define LED0_PIN    GET_PIN(C, 13)

int main(void)
{
    LOG_D("Hello RT-Thread!");
    rt_pin_mode(LED0_PIN, PIN_MODE_OUTPUT);

    for(;;)
    {
        rt_pin_write(LED0_PIN, PIN_HIGH);
        rt_thread_mdelay(500);
        rt_pin_write(LED0_PIN, PIN_LOW);
        rt_thread_mdelay(500);
    }

    return RT_EOK;
}
```

- 实例代码运行现象：板载的 LED 进行周期性闪烁。

## PIN 设备相关软件包使用

1. 我们使用 [LedBlink 软件包](http://packages.rt-thread.org/detail.html?package=LedBlink) 来验证 PIN 设备接口。

2. 首先克隆 LedBlink 软件包到 STM32L431RCT6 的 RT-Thread Nano 工程。LedBlink 软件包链接：https://github.com/aeo123/LedBlink.git

3. 使用 LedBlink 软件包实例：

```c
#include <rtthread.h>

#define DBG_TAG "main"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

#include "drv_gpio.h"
#include "ledblink.h"
#define LED0_PIN    GET_PIN(C, 13)

int main(void)
{
    int count = 1;

    LOG_D("Hello RT-Thread!");
    led_add_device(LED0_PIN);

    while (count++)
    {
        led_on(1);
        rt_thread_mdelay(500);
        led_off(1);
        rt_thread_mdelay(500);
    }

    return RT_EOK;
}
```
4. LedBlink 软件包无需任何修改便可在 RT-Thread Nano 工程中使用。
5. 实例代码运行现象：板载的 LED 进行周期性闪烁。

