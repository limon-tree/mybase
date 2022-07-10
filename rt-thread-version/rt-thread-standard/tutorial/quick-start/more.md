# 其他开发板 上手指南

## 新手推荐开发板列表及其 BSP

推荐没有 RTOS 基础的新手选择以下一款开发板学习，按照文档中心的快速上手文档进行快速上手：

| 厂商     | 开发板                                                       |
| -------- | ------------------------------------------------------------ |
| 正点原子 | 【强烈推荐】[潘多拉（IoT Board）STM32L475](../quick-start/iot_board/quick-start.md) |
|          | 【推荐】[nano STM32F103](../quick-start/stm32f103-atk-nano/quick-start.md) |
|          | 【推荐】[探索者 STM32F407](../quick-start/stm32f407-atk-explorer/quick-start.md) |
|          | 【推荐】[阿波罗 STM32F429](../quick-start/stm32f429-atk-apolo/quick-start.md) |
| 野火     | 【推荐】[霸道 STM32F103](../quick-start/stm32f103-fire-arbitrary/quick-start.md) |
|          | 【推荐】[挑战者 STM32F429](../quick-start/stm32f429-fire-challenger/quick-start.md) |
| 新唐     | 【推荐】[Nuvoton NuMaker-PFM-M487](../quick-start/numaker-pfm-m487/quick-start.md) |
|          | 【推荐】[Nuvoton NuMaker-IoT-M487](../quick-start/numaker-iot-m487/quick-start.md) |
|          | 【推荐】[Nuvoton NK-980IOT](../quick-start/nk-980iot/quick-start.md) |
|          | 【推荐】[Nuvoton NuMaker-M2354](../quick-start/numaker-m2354/quick-start.md) |
|          | 【推荐】[Nuvoton NK-RTU980](../quick-start/nk-rtu980/quick-start.md) |
|          | 【推荐】[Nuvoton NK-N9H30](../quick-start/nk-n9h30/quick-start.md) |
|          | 【推荐】[Nuvoton NuMaker-M032KI](../quick-start/numaker-m032ki/quick-start.md) |

## 其他开发板及其 BSP

RT-Thread 支持的全部 BSP 均在 github 的 `rt-thread` 源码中，位于 `bsp` 文件夹下，可以下载 [RT-Thread 源码](https://github.com/RT-Thread/rt-thread)，按照相应 BSP 下的 README 文档进行快速上手。下列为部分开发板：

| 厂商     | 开发板           |
| -------- | ---------------- |
| 正点原子 | [号令者 I.MX RT1052](https://github.com/RT-Thread/rt-thread/tree/master/bsp/imxrt/imxrt1052-atk-commander) |
|          | [阿波罗 STM32F767](https://github.com/RT-Thread/rt-thread/tree/master/bsp/stm32/stm32f767-atk-apollo) |
| 野火 | [I.MX RT1052](https://github.com/RT-Thread/rt-thread/tree/master/bsp/imxrt/imxrt1052-fire-pro) |
|      | [挑战者 STM32F767](https://github.com/RT-Thread/rt-thread/tree/master/bsp/stm32/stm32f767-fire-challenger) |
| NXP | [I.MX RT1050 EVK](https://github.com/RT-Thread/rt-thread/tree/master/bsp/imxrt/imxrt1052-nxp-evk) |
| | [NXP LPC54110 Board](https://github.com/RT-Thread/rt-thread/tree/master/bsp/lpc54114-lite) |
| | [LPC54608 系列评估板](https://github.com/RT-Thread/rt-thread/tree/master/bsp/lpc54608-LPCXpresso) |
| 兆易创新 | [GD32303E-EVAL](https://github.com/RT-Thread/rt-thread/tree/master/bsp/gd32303e-eval) |
| | [GD32450Z-EVAL](https://github.com/RT-Thread/rt-thread/tree/master/bsp/gd32450z-eval) |
| RISC-V | [SiFive HIFIVE1](https://github.com/RT-Thread/rt-thread/tree/master/bsp/hifive1) |
| 华芯微特 | [SWXT-LQ100-32102](https://github.com/RT-Thread/rt-thread/tree/master/bsp/swm320-lq100) |
| Nordic | [nRF52840-PCA10056](https://github.com/RT-Thread/rt-thread/tree/master/bsp/nrf5x/nrf52840) |
|  | [nRF52832-PCA10040](https://github.com/RT-Thread/rt-thread/tree/master/bsp/nrf5x/nrf52832) |

## STM32 系列 BSP 制作

RT-Thread 团队提供  [STM32 系列 BSP 制作教程](https://github.com/RT-Thread/rt-thread/blob/master/bsp/stm32/docs/STM32%E7%B3%BB%E5%88%97BSP%E5%88%B6%E4%BD%9C%E6%95%99%E7%A8%8B.md)，若目标开发板是 STM32 系列，但在 RT-Thread 源码中没有找到相应的 BSP，那么可以根据这里提供的教程制作一个新的 BSP。
