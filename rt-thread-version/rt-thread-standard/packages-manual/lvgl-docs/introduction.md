# LVGL 介绍

[LVGL](https://www.lvgl.io/)是一款高度可裁剪、界面符合现代审美、简单易用的开源免费嵌入式图形库（MIT 协议），创始人是来自匈牙利的 Gábor Kiss-Vámosi。目前 LVGL 源码托管在 [Github](https://github.com/lvgl/lvgl) 平台上进行维护，在社区成员 [满鉴霆](https://github.com/mysterywolf) 的推动下，RT-Thread 社区与 LVGL 社区已经完成了源码对接，LVGL 官方 Github 源码仓库已经成为 RT-Thread 的软件包，RT-Thread 社区小伙伴只需要使用 Env 工具或者 RT-Studio 即可拉取到 LVGL 的最新源码，并自动加入到 RT-Thread 工程中。

## 已经适配 LVGL 的 BSP

[已经适配LVGL的BSP列表](https://docs.lvgl.io/master/get-started/os/rt-thread.html#how-to-run-lvgl-on-rt-thread) | [讲解视频](https://www.bilibili.com/video/BV1YM4y1F7fX)

RT-Thread 的两款模拟器均适配了 LVGL，你可以不需要开发板也可以在 RT-Thread 操作系统上运行 LVGL 图形库。在已经适配好的 BSP 中，用户一键即可完成 LVGL 的配置并生成工程，编译下载后，自动演示 LVGL 的例程。如何配置请参考文档或（及）讲解视频。

## 如何将 LVGL 移植到某个 BSP

移植模板请参见 [正点原子 STM32L475 潘多拉](https://github.com/RT-Thread/rt-thread/tree/master/bsp/stm32/stm32l475-atk-pandora/applications/lvgl) | [STM32F469 Discovery](https://github.com/RT-Thread/rt-thread/tree/master/bsp/stm32/stm32f469-st-disco/applications/lvgl) | [新唐系列](https://github.com/RT-Thread/rt-thread/blob/master/bsp/nuvoton/docs/LVGL_Notes.md)

LVGL 的配置文件有三个：

| 配置文件名称              | 位置                                                                                                                               | 功能                       | 用户是否需要修改此文件                              |
| ------------------- | -------------------------------------------------------------------------------------------------------------------------------- | ------------------------ | ---------------------------------------- |
| lv_conf_internal.h  | [LVGL 侧](https://github.com/lvgl/lvgl/blob/master/src/lv_conf_internal.h)                                                        | 将用户未配置的功能设置为默认值          | 不需要                                      |
| lv_rt_thread_conf.h | [LVGL 侧](https://github.com/lvgl/lvgl/blob/master/env_support/rt-thread/lv_rt_thread_conf.h)                                     | 接管与操作系统相关的配置（例如时基信号、内存等） | 不需要                                      |
| lv_conf.h           | RT-Thread 侧：[参考](https://github.com/RT-Thread/rt-thread/blob/master/bsp/stm32/stm32l475-atk-pandora/applications/lvgl/lv_conf.h) | 用户自定义配置                  | 需要用户自行在具体 BSP 的 applications/lvgl 文件夹下创建 |

从上表中可以看出，虽然配置文件看起来三个很多，但是实际需要用户干预的只有最后一项，即用户需要在 BSP 的 applications 文件夹中创建一个 lvgl 文件夹，该文件夹就是用来放置 lvgl 相关文件的，内容如下：

| 文件名称            | 作用                                                                                                                                                                                                                                                                      |
| --------------- | ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| lv_conf.h       | 用户自定义配置 LVGL 功能（该文件名不可以乱改）                                                                                                                                                                                                                                              |
| lv_port_disp.c  | LVGL 显示屏对接文件，在 `void lv_port_disp_init(void)` 的函数内初始化与LVGL显示框架对接相关内容（该函数必须存在）。[官方手册参考](https://docs.lvgl.io/master/porting/display.html) \| [移植参考](https://github.com/RT-Thread/rt-thread/blob/master/bsp/stm32/stm32f469-st-disco/applications/lvgl/lv_port_disp.c)    |
| lv_port_indev.c | LVGL 输入设备对接文件，在 `void lv_port_indev_init(void)` 的函数内初始化与LVGL输入设备框架对接相关内容（该函数必须存在）。[官方手册参考](https://docs.lvgl.io/master/porting/indev.html) \| [移植参考](https://github.com/RT-Thread/rt-thread/blob/master/bsp/stm32/stm32f469-st-disco/applications/lvgl/lv_port_indev.c) |
| 用户界面初始化文件       | 在 `void lv_user_gui_init(void)` 函数内调用你想要初始化的函数（该函数必须存在），LVGL在初始化时会自动调用该函数。[移植参考](https://github.com/RT-Thread/rt-thread/blob/master/bsp/stm32/stm32f469-st-disco/applications/lvgl/demo/lv_demo.c)                                                                      |

> 自 RT-Thread 4.1.1 版本之后（含4.1.1），用户无需再创建 `lv_port_disp.h` 和 `lv_port_indev.h` 文件。

用户无需关心LVGL初始化的问题，LVGL会在设备上电时自动由RT-Thread初始化，同时也会自动调用 `lv_port_disp_init`、`lv_port_indev_init` 和 `lv_user_gui_init`来初始化用户的显示屏、输入设备驱动以及界面绘制函数。这些初始化的工作已经在 LVGL 侧的 [lv_rt_thread_port.c](https://github.com/lvgl/lvgl/blob/master/env_support/rt-thread/lv_rt_thread_port.c) 文件中自动初始化完成。

关于是否需要对接RT-Thread显示设备框架与触摸框架的问题，用户可以自由处理，如果不愿意对接RT-Thread显示和触摸框架，可以直接将驱动怼到LVGL侧的对接函数上也是没有问题的。[移植参考](https://github.com/RT-Thread/rt-thread/blob/master/bsp/stm32/stm32l475-atk-pandora/applications/lvgl/lv_port_disp.c)

## 提问

- 如果遇到移植相关的问题请到 RT-Thread 社区论坛提问：https://club.rt-thread.org/index.html

- 如果遇到 LVGL 使用相关的问题请到 LVGL 社区论坛提问（请使用英文）：https://forum.lvgl.io/
