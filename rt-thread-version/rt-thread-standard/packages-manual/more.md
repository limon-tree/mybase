# 软件包分类

以下列出一些常用的软件包，更多软件包还请访问 [RT-Thread 软件包中心](http://packages.rt-thread.org/) 进行查看和使用。社区也提供了 [Python脚本](https://github.com/supperthomas/rtthread_software_package_list_show) 可以自动生成现有软件包的列表。

## IoT

与物联网相关的软件包，包括网络相关软件包，云接入软件包等。

| 软件包名称                                                   | 备注                                                  |
| ------------------------------------------------------------ | ----------------------------------------------------- |
| [ota_downloader](http://packages.rt-thread.org/detail.html?package=ota_downloader) | 基于RT-Thread OTA 组件的 固件下载器                   |
| [onenet](http://packages.rt-thread.org/detail.html?package=onenet) | 连接中国移动 OneNet 云的软件包                        |
| [webclient](http://packages.rt-thread.org/detail.html?package=webclient) | RT-Thread 官方开源的 http/https 协议客户端            |
| [webnet](http://packages.rt-thread.org/detail.html?package=webnet) | RT-Thread 官方开源的、轻量级、可定制嵌入式 Web 服务器 |
| [webTerminal](http://packages.rt-thread.org/detail.html?package=WebTerminal) | 可以在浏览器上运行的终端                              |
| [rw007](http://packages.rt-thread.org/detail.html?package=rw007) | RT-Thread 的 RW007 驱动 （SPI Wi-Fi 模式）            |
| [pahomqtt](http://packages.rt-thread.org/detail.html?package=pahomqtt) | Eclipse 开源的 MQTT C/C++ 客户端                      |
| [netutils](http://packages.rt-thread.org/detail.html?package=netutils) | RT-Thread 网络小工具集                                |
| [at_device](http://packages.rt-thread.org/detail.html?package=at_device) | AT 组件在不同设备上的移植或示例                       |

[更多IOT相关软件包...](http://packages.rt-thread.org/search.html?classify=iot)

## 外设

与底层外设硬件相关的软件包，sensor软件包等

| 软件包名称                                                   | 备注                                                         |
| ------------------------------------------------------------ | ------------------------------------------------------------ |
| [aht10](http://packages.rt-thread.org/detail.html?package=aht10) | 数字温湿度传感器 aht10 的驱动库                              |
| [dht11](http://packages.rt-thread.org/detail.html?package=dht11) | DHT11 单总线数字温湿度传感器                                 |
| [gt9147](http://packages.rt-thread.org/detail.html?package=gt9147) | GT9147 触摸芯片的驱动包                                      |
| [ft6236](http://packages.rt-thread.org/detail.html?package=ft6236) | FT6236 触摸芯片的驱动包                                      |
| [mpu6xxx](http://packages.rt-thread.org/detail.html?package=mpu6xxx) | 兼容 mpu6000, mpu6050, mpu6500, mpu9250 等等型号的驱动库     |
| [pcf8574](http://packages.rt-thread.org/detail.html?package=pcf8574) | 针对 I2C 并行口扩展 8 位 I/O 软件包                          |
| [easyblink](http://packages.rt-thread.org/detail.html?package=easyblink) | 小巧轻便的 LED 控制软件包, 可以容易地控制 LED 开 、关、反转和各种间隔闪烁 |
| [max17048](http://packages.rt-thread.org/detail.html?package=max17048) | 电池监测芯片                                                 |
| [ds18b20](http://packages.rt-thread.org/detail.html?package=ds18b20) | 单总线数字温湿度传感器 ds18b20 的软件包                      |

[更多外设相关软件包...](http://packages.rt-thread.org/search.html?classify=peripherals)

## 系统

系统级软件包，监控系统行为、其他文件系统等

| 软件包名称                                                   | 备注                                                 |
| ------------------------------------------------------------ | ---------------------------------------------------- |
| [CMSIS](http://packages.rt-thread.org/detail.html?package=CMSIS) | CMSIS  软件包在 RT-Thread 上的移植                   |
| [FlashDB](http://packages.rt-thread.org/detail.html?package=FlashDB) | 一款支持 KV 数据和时序数据的轻量级数据库             |
| [fal](http://packages.rt-thread.org/detail.html?package=fal) | Flash 抽象层的实现，负责管理 Flash 设备和 Flash 分区 |
| [littlefs](http://packages.rt-thread.org/detail.html?package=littlefs) | 为微控制器设计的一个小型的且掉电安全的文件系统       |
| [syswatch](http://packages.rt-thread.org/detail.html?package=syswatch) | 系统看守：一个用于保障系统长期正常运行的组件         |
| [rt_printf](http://packages.rt-thread.org/detail.html?package=rt_printf) | 线程安全版本的rt_kprintf                             |
| [LittlevGL2RTT](http://packages.rt-thread.org/detail.html?package=LittlevGL2RTT) | LittlevGL2RTT 是基于 RT-Thread 的图形库软件包        |

[更多系统相关软件包...](http://packages.rt-thread.org/search.html?classify=system)

## 编程语言

可运行在终端板卡上的各种编程语言，脚本或解释器

| 软件包名称                                                   | 备注                                                         |
| ------------------------------------------------------------ | ------------------------------------------------------------ |
| [jerryscript](http://packages.rt-thread.org/detail.html?package=jerryscript) | 针对 RT-Thread 的JerryScript 移植                            |
| [Lua](http://packages.rt-thread.org/detail.html?package=Lua) | Lua 库适配 RT-Thread 3.0 (基于 lua 5.1.4版本 和 lua 5.3.4版本) |
| [micropython](http://packages.rt-thread.org/detail.html?package=micropython) | MicroPython 在 RT-Thread 上的移植                            |

[更多编程语言相关软件包...](http://packages.rt-thread.org/search.html?classify=language)

## 工具

辅助使用的一些工具软件包

| 软件包名称                                                   | 备注                                                     |
| ------------------------------------------------------------ | -------------------------------------------------------- |
| [adbd](http://packages.rt-thread.org/detail.html?package=adbd) | 在 RT-Thread 上实现的 Android ADB daemon                 |
| [CmBacktrace](http://packages.rt-thread.org/detail.html?package=CmBacktrace) | ARM Cortex-M 系列 MCU 错误追踪库                         |
| [EasyFlash](http://packages.rt-thread.org/detail.html?package=EasyFlash) | 轻量级嵌入式 Flash 存储器库，让 Flash 成为小型 KV 数据库 |
| [EasyLogger](http://packages.rt-thread.org/detail.html?package=EasyLogger) | 一款超轻量级(ROM<1.6K, RAM<0.3k)、高性能的 C/C++ 日志库  |
| [SystemView](http://packages.rt-thread.org/detail.html?package=SystemView) | SEGGER 的 SystemView 移植                                |
| [ulog_easyflash](http://packages.rt-thread.org/detail.html?package=ulog_easyflash) | 基于 EasyFlash 的 ulog 插件                              |

[更多工具相关软件包...](http://packages.rt-thread.org/search.html?classify=tools)

## 杂类

一些未归类的软件包，demo，示例等

| 软件包名称                                                   | 备注                                       |
| ------------------------------------------------------------ | ------------------------------------------ |
| [fastlz](http://packages.rt-thread.org/detail.html?package=fastlz) | 一款极速的压缩库                           |
| [filesystem_samples](http://packages.rt-thread.org/detail.html?package=filesystem_samples) | RT-Thread 文件系统示例                     |
| [network_samples](http://packages.rt-thread.org/detail.html?package=network_samples) | RT-Thread 网络示例                         |
| [peripheral_samples](http://packages.rt-thread.org/detail.html?package=peripheral_samples) | RT-Thread 外设示例                         |
| [crclib](http://packages.rt-thread.org/detail.html?package=crclib) | 一个包含8位、16位、32位CRC校验计算的函数库 |

[更多杂类相关软件包...](http://packages.rt-thread.org/search.html?classify=misc)

## 多媒体

RT-Thread上的音视频软件包

| 软件包名称                                                   | 备注                                          |
| ------------------------------------------------------------ | --------------------------------------------- |
| [TJpgDec](http://packages.rt-thread.org/detail.html?package=TJpgDec) | JPEG 解码库                                   |
| [touchgfx2rtt](http://packages.rt-thread.org/detail.html?package=touchgfx2rtt) | touchgfx在RT-Thread上的移植。                 |
| [wavplayer](http://packages.rt-thread.org/detail.html?package=wavplayer) | 简洁的wav格式的音乐播放器，提供播放和录音功能 |
| [STemWin](http://packages.rt-thread.org/detail.html?package=STemWin) | STemWin在RT-Thread上的移植                    |

[更多多媒体相关软件包...](http://packages.rt-thread.org/search.html?classify=multimedia)

## 安全

加解密算法及安全传输层

| 软件包名称                                                   | 备注                                                         |
| ------------------------------------------------------------ | ------------------------------------------------------------ |
| [mbedtls](http://packages.rt-thread.org/detail.html?package=mbedtls) | 一个开源的、可移植的、易于使用的、可读的且灵活的 SSL 库      |
| [tinycrypt](http://packages.rt-thread.org/detail.html?package=tinycrypt) | 一个简小并且可配置的加解密软件包                             |
| [yd_crypto](http://packages.rt-thread.org/detail.html?package=yd_crypto) | 用于微控制器的加解密算法库，平台无关、算法独立、易移植、易使用。 |

[更多安全相关软件包...](http://packages.rt-thread.org/search.html?classify=security)
