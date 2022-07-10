# USB 设备

USB 设备的配置项说明如下，用于配置设备描述符，配置描述符与字符串描述符。

| 配置项名称        | 说明                                                         |
| ----------------- | ------------------------------------------------------------ |
| Device Vendor ID  | 供应商 ID（VID，由 USB-IF 分配）                             |
| Device Product ID | 产品 ID（PID，由制造商分配）                                 |
| Manufacturer name | 制造商描述                                                   |
| Device name       | 设备描述                                                     |
| Endpoint 0 size   | 端点 0 最大数据包大小（数据包的大小可以为 8 Bytes、16 Bytes、32 个 Bytes 和 64 Bytes） |
| Maximum current   | 设备所需的最大电流                                           |

# CDC

**通信设备类 (CDC)** 实现了虚拟串口 (Virtual COM Port) 的功能，用户可以使用 CDC 设备模拟串口，与主机进行通信。

## 示例代码

```c
#include <stdint.h>
#include <rtthread.h>
#include <tusb.h>

void cdc_example(void)
{
    uint8_t buffer[32];
    char ch;
	
    /* 判断端口是否打开 (DTR=1) */
    if (tud_cdc_connected())
    {
        rt_memset(buffer, 0, 32);
		
        /* 发送字符串 */
        tud_cdc_write_str("please enter something: ");
        /* 刷新发送缓冲区 */
        tud_cdc_write_flush();
        /* 清空读取缓冲区 */
        tud_cdc_read_flush();

        for (int i = 0; i < 32; i++)
        {
            /* 判断当前是否有数据可读取 */
            while (!tud_cdc_available()) { rt_thread_mdelay(10); }
            /* 读取一个字符 */
            ch = tud_cdc_read_char();
            *(buffer + i) = ch;
            /* 字符回显 */
            tud_cdc_write_char(ch);
            tud_cdc_write_flush();
            /* 按下回车结束输入 */
            if (ch == '\r') { break; }
        }
        tud_cdc_write_str("\r\nwhat you enter: ");
        tud_cdc_write((const char *) buffer, 32);
        tud_cdc_write_str("\r\n");
        tud_cdc_write_flush();
    }
    else
    {
        rt_kprintf("please open port and make sure DTR=1\n");
    }
}
MSH_CMD_EXPORT(cdc_example, TinyUSB cdc example)
```

## 配置项说明

| 配置项名称                   | 说明                                              |
| ---------------------------- | ------------------------------------------------- |
| Description of CDC           | CDC 设备描述                                      |
| Notification endpoint number | 通知端点编号（Interrupt 类型，IN 端点）           |
| Endpoint number              | 数据传输端点编号（Bulk 类型，IN、OUT 共两个端点） |
| Rx buffer size               | 接收缓冲区大小                                    |
| Tx buffer size               | 发送缓冲区大小                                    |

# MSC

**大容量存储类 (MSC)** 主要用于访问设备的内部存储，常见的大容量存储设备有：

- 外部硬盘
- 外部 CD/DVD
- U 盘
- 固态硬盘
- 读卡器

对于 MSC，软件包已将相关函数与设备驱动框架对接，用户通常仅需编写少量代码，并指定用于大容量存储的块设备名称即可。

> [!NOTE]
> 注意：当块设备被挂载期间，由于 USB 主机与内核没有进行同步，需注意用户代码不能对块设备进行任何操作，否则会造成 **竞争**。在设备被挂载之前，应调用 `rt_device_close()` 关闭设备。并在设备卸载后重新打开块设备。如需要在设备挂载期间仍能访问块设备，应使用 MTP 协议（TinyUSB 暂不支持）。

## 配置项说明

| 配置项名称                               | 说明                                                         |
| ---------------------------------------- | ------------------------------------------------------------ |
| Description of MSC                       | MSC 设备描述                                                 |
| SCSI VID                                 | SCSI 供应商 ID                                               |
| SCSI PID                                 | SCSI 产品 ID                                                 |
| SCSI Product revision                    | SCSI 产品版本                                                |
| Endpoint number                          | 端点编号（Bulk 类型，IN、OUT 共两个端点）                    |
| Endpoint buffer size                     | 端点缓冲区大小，必须大于块设备扇区大小，通常 SPI Nor Flash 为 4 KBytes，SD 卡为 512 Bytes |
| The name of the block device used by MSC | 用于大容量存储设备的块设备名称                               |

# HID

**人机接口设备（HID）** 通常用于允许人类与 PC 交互的设备。使用这些设备，主机能够对人类的输入（如鼠标的移动或按键）作出反应。这种反应必须迅速发生，以便用户不会注意到他的行动和预期的反馈之间的明显延迟。TinyUSB 预定义的 HID 设备如下：

- 鼠标
- 键盘
- 游戏手柄
- 用户控制设备

此外，还可自定义报文描述符用于特定应用程序。

## 示例代码

```c
#include <stdarg.h>
#include <stdint.h>
#include <math.h>

#include <rtthread.h>
#include <tusb.h>
#include <usb_descriptor.h>

#define CIRCLE_RADIUS 300

/* 字符键值转换表 */
static const uint8_t conv_table[128][2] =  { HID_ASCII_TO_KEYCODE };
static struct rt_semaphore hid_sem;

/* 报文发送完毕回调函数 */
void tud_hid_report_complete_cb(uint8_t instance, uint8_t const* report, uint8_t len)
{
    (void) instance;
    (void) report;
    (void) len;
	/* 使用信号量进行同步，确保报文不会因发送过快导致还未发送的报文被覆盖 */
    rt_sem_release(&hid_sem);
}

static void hid_press_key(uint8_t modifier, uint8_t keycode)
{
    uint8_t keycode_array[6] = {0};

    keycode_array[0] = keycode;
    tud_hid_keyboard_report(REPORT_ID_KEYBOARD, modifier, keycode_array);
    rt_sem_take(&hid_sem, 100);
    keycode_array[0] = 0;
    tud_hid_keyboard_report(REPORT_ID_KEYBOARD, 0, keycode_array);
    rt_sem_take(&hid_sem, 100);
}

static void hid_type_str_fmt(const char *fmt, ...)
{
    uint8_t  keycode;
    uint8_t  modifier;
    uint32_t length;
    va_list  args;
    char     buffer[64];
    char     chr;

    va_start(args, fmt);
    rt_vsnprintf(buffer, 64, fmt, args);

    length = rt_strlen(buffer);
    for (int i = 0; i < length; i++)
    {
        chr = buffer[i];
        if (conv_table[chr][0])
        {
            modifier = KEYBOARD_MODIFIER_LEFTSHIFT;
        }
        else
        {
            modifier = 0;
        }
        keycode = conv_table[chr][1];
        hid_press_key(modifier, keycode);
    }
}

void hid_example(void)
{
    float last_x_pos = CIRCLE_RADIUS, x_pos;
    float last_y_pos = 0, y_pos;

    /* 初始化信号量 */
    rt_sem_init(&hid_sem, "hid", 0, RT_IPC_FLAG_PRIO);

    if (tud_connected())
    {
        /* 用鼠标画个圆 */
        for (int i = 0; i < 360; i++)
        {
            x_pos = cosf((float)i / 57.3F) * CIRCLE_RADIUS;
            y_pos = sinf((float)i / 57.3F) * CIRCLE_RADIUS;

            tud_hid_mouse_report(REPORT_ID_MOUSE, 0,
                                 (int8_t)(x_pos - last_x_pos), (int8_t)(y_pos - last_y_pos), 0, 0);
            rt_sem_take(&hid_sem, 100);

            last_x_pos = x_pos;
            last_y_pos = y_pos;
        }
        rt_thread_mdelay(500);

        /* 打开记事本并输入一些文字 */
        hid_press_key(KEYBOARD_MODIFIER_LEFTGUI, HID_KEY_R);
        rt_thread_mdelay(500);
        hid_type_str_fmt("notepad\n\n");
        rt_thread_mdelay(2000);
        hid_type_str_fmt("This is RT-Thread TinyUSB demo.\n");
    }
    else
    {
        rt_kprintf("please connect USB port\n");
    }

    rt_sem_detach(&hid_sem);
}
MSH_CMD_EXPORT(hid_example, TinyUSB hid example)
```

## 配置项说明

| 配置项名称            | 说明                                |
| --------------------- | ----------------------------------- |
| Description of HID    | HID 设备描述                        |
| Endpoint number       | 端点编号（Interrupt 类型，IN 端点） |
| Polling interval (ms) | 轮询间隔，单位为 **毫秒 (ms)**      |
| Keyboard report       | 使能键盘报文类型                    |
| Mouse report          | 使能鼠标报文类型                    |
| Gamepad report        | 使能游戏手柄报文类型                |
| Endpoint buffer size  | 端点缓冲区大小                      |

