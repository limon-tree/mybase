# 使用NTP轻松获取网络时间

<iframe frameborder="0" width="1005px" height="663px" src="https://v.qq.com/txp/iframe/player.html?vid=l0765nw5r7y" allowFullScreen="true"></iframe>

> 提示：<a href="../ntp.pdf" target="_blank">视频 PPT 下载</a>

## 背景介绍

[NTP](https://baike.baidu.com/item/NTP) 网络时间协议(Network Time Protocol)，是用来同步网络中各个计算机时间的协议。

本教程介绍了如何开启 NTP 软件包（一个运行在 RT_Thread 上的 NTP 客户端），并且当连接上网络后，如何利用这个软件包，获取当前的 UTC 时间，并更新至 RTC（实时时钟）中。

## 获取示例代码 ##

打开 Env 工具输入 menuconfig 按照下面的路径即可打开 ntp 客户端

```
 RT-Thread online packages  --->
     IoT - internet of things  --->
         netutils: Networking utilities for RT-Thread  --->
             [*]   Enable NTP(Network Time Protocol) client
```

## 在 msh shell 中运行代码

系统运行起来后，在 msh 命令行下输入 ntp_sync 命令即可通过 NTP 协议获取当前的 UTC 时间，并更新至 RTC（实时时钟）中。

```
msh> ntp_sync
```

## 预期结果 ##

如果程序能够正常运行，就会获取到正确的网络时间，运行结果如下

```
Get local time from NTP server: Wed Jul 11 17:19:02 2018
The system time is updated. Timezone is 8.
```

> [!NOTE]
> 注：* 如出现连接失败的情况，请尝试关闭防火墙

## 参考资料

* 源码 [ntp](https://github.com/RT-Thread-packages/netutils/tree/master/ntp)
* [RT-Thread 网络小工具集](https://github.com/RT-Thread-packages/netutils)
* [《Env 用户手册》](../../../programming-manual/env/env.md)

## 常见问题

* [常见问题及解决方法](../faq/faq.md)。
