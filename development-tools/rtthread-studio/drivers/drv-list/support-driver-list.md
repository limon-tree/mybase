# RT-Thread Studio 驱动支持概况

本文将介绍 RT-Thread Studio 对于硬件外设驱动的支持情况。由于 RT-Thread nano 版本不具有设备概念，所以本文提到的 RT-Thread 为完整版本。nano 版建议直接使用 HAL 库进行驱动开发。

## 驱动概况

当前 RT-Thread Studio 驱动开发方式主要分为二种

- 1、由 RT Thread Studio 自动生成，无需修改任何文件或者简单定义几个宏即可直接使用的驱动，如 GPIO，UART，I2C，SPI，SDIO 和 ETH 等。

- 2、没有对接到设备驱动框架，可直接使用 HAL 库函数进行开发的驱动，如 DAC，FSMC 等。

RT-Thread Studio （截止V1.0.4）的驱动支持情况见下表

| 外设       | Studio 自动生成       | HAL 库函数开发 |
|------------|-----------------------|----------------|
| PIN        | 支持                  | 支持           |
| UART       | 支持                  | 支持           |
| I2C        | 支持                  | 支持           |
| SPI        | 支持                  | 支持           |
| QSPI       | 支持                  | 支持           |
| WDT        | 支持                  | 支持           |
| PWM        | 支持                  | 支持           |
| RTC        | 支持                  | 支持           |
| FLASH      | 支持                  | 支持           |
| SDIO       | 支持                  | 支持           |
| USB HOST   | 支持                  | 支持           |
| USB DEVICE | 支持                  | 支持           |
| ETH        | 支持                  | 支持           |
| TIMER      | 支持(推荐使用 HAL 库) | 支持           |
| ADC        | 支持(推荐使用 HAL 库) | 支持           |
| DAC        | 不支持                | 支持           |
| FSMC       | 不支持                | 支持           |
| CAN        | 不支持                | 支持           |

## 使用简介

### Studio 自动生成的驱动

使用 RT-Thread Studio 新建完整版工程时，用户不需要修改任何代码，例如：

- PIN ：该驱动对接到了设备驱动框架并且可以直接使用，具体 PIN 的使用参考 [PIN设备](https://www.rt-thread.org/document/site/#/development-tools/rtthread-studio/drivers/pin/rtthread-studio-pin)

- UART ：对于串口驱动来说，生成工程的时候串口驱动也自动对接到了设备驱动框架，使用时需要自己定义串口号以及串口引脚对应的宏，具体参考 [串口设备](https://www.rt-thread.org/document/site/#/development-tools/rtthread-studio/drivers/uart/v4.0.2/rtthread-studio-uart-v4.0.2)

- I2C ：软件 I2C 的驱动在使用 RT-Thread Studio 自动生成工程时也对接到了设备驱动框架，软件 I2C 驱动的开发参考 [软件模拟I2C设备](https://www.rt-thread.org/document/site/#/development-tools/rtthread-studio/drivers/soft-i2c/rtthread-studio-soft-i2c)

- SPI ：SPI 设备驱动的开发参考 [SPI设备](https://www.rt-thread.org/document/site/#/development-tools/rtthread-studio/drivers/spi/rtthread-studio-spi)

- 更多设备：更多设备驱动的开发请参考 RT-Thread 官网文档中心的相关章节。

> 提示：虽然 ADC 及 Timer 外设已经对接到了设备驱动框架，但是其功能不够完善，例如：不支持 DMA，定时器的一些高级功能无法使用等。对于这类驱动，目前推荐使用 HAL 库函数方式进行开发。

### 直接使用 HAL 库函数开发的驱动

虽然可以通过 Studio 自动或者手动方式添加设备驱动代码，但是还是有些外设暂时无法对接到设备框架

此时用户直接按照 HAL 库开发的方式，开发对应的驱动即可。具体开发方式可以参考[RT-Thread Studio 结合 STM32CubeMx 开发其他驱动文档](https://www.rt-thread.org/document/site/#/development-tools/rtthread-studio/drivers/cubemx/rtthread-studio-cubemx)
