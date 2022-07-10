# 正点原子探索者 STM32F407 上手指南

## 简介

探索者 STM32F407 是正点原子推出的一款基于 ARM Cortex-M4 内核的开发板，最高主频为 168Mhz，该开发板具有丰富的板载资源，可以充分发挥 STM32F407 的芯片性能。

开发板外观如下图所示：

![board](figures/board.jpg)

该开发板常用 **板载资源** 如下：

  - MCU：STM32F407ZGT6，主频 168MHz，1024KB FLASH ，192KB RAM
- 外部 RAM：IS62WV51216（1MB）
- 外部 FLASH：W25Q128（SPI，16MB）
- 常用外设
    * LED：2个，DS0（红色，PB1），DS1（绿色，PB0）
    * 按键，4个，KEY_UP（兼具唤醒功能，PIN：0），K0（PIN：68），K1（PIN：67），K2（PIN：66）
- 常用接口：USB 转串口、SD 卡接口、以太网接口、LCD 接口、USB SLAVE、USB HOST
- 调试接口，标准 JTAG/SWD

开发板更多详细信息请参考 [正点原子官方品牌店宝贝介绍](https://eboard.taobao.com/index.htm)。

## 准备工作

正点原子探索者 STM32F407 板级支持包提供 MDK4、MDK5 和 IAR 工程，并且支持 GCC 开发环境，下面以 MDK5 开发环境为例，介绍如何将示例程序运行起来。运行示例程序前需要做如下准备工作：

 1. MDK 开发环境

     需要安装 MDK-ARM 5.24 (正式版或评估版，5.14 版本及以上版本均可)，这个版本也是当前比较新的版本，它能够提供相对比较完善的调试功能。安装方法可以参考 [Keil MDK 安装](../keil/keil.md)。

 2. 源码获取

     <https://github.com/RT-Thread/rt-thread/>

     [源码目录说明](../src_code_introduction/rtthread_dir.md)

 3. 使用 Mini USB 线连接开发板的 USB 转串口 到 PC 机。

     ![连接到 PC](figures/usb_pc.jpg)

## 运行第一个示例程序

### 编译下载

进入到 `rt-thread\bsp\stm32\stm32f407-atk-explorer` 文件夹中，双击 project.uvprojx 文件，打开 MDK5 工程。

![工程目录](figures/dir.jpg)

执行编译，编译完成后，点击下载按钮将固件下载至开发板，下载完成后，程序会自动开始运行，观察程序运行状况。

> 提示：工程默认配置使用 JLink 下载程序，在通过 JLink 连接开发板的基础上，点击下载按钮即可下载程序到开发板

![编译下载方法](figures/project.jpg)

### 运行

如没有自动运行，按下复位按键重启开发板，观察开发板上 LED 的实际效果。正常运行后，LED 灯会周期性闪烁，如下图所示：

![run](figures/run.gif)

连接开发板对应串口到 PC , 在串口工具里打开相应的串口(115200-8-1-N) ，复位设备后，可以看到 RT-Thread 的输出信息:

> 提示：注：正点原子一键下载电路和终端工具冲突，在使用终端工具如：PuTTy、XShell 时，会出现系统不能启动的问题，推荐使用串口调试助手如：sscom

```bash
 \ | /
- RT -     Thread Operating System
 / | \     3.1.1 build Nov 19 2018
 2006 - 2018 Copyright by rt-thread team
msh >
```

## 继续学习

已完成 RT-Thread 快速上手！点击这里进行 [内核学习](../../kernel/kernel-video.md) 。
