# 使用 QEMU 运行 RT-Thread

<iframe frameborder="0" width="1005px" height="663px" src="https://v.qq.com/txp/iframe/player.html?vid=i0759ofkhux" allowFullScreen="true"></iframe>

> 提示：<a href="../qemu_setup.pdf" target="_blank">视频 PPT 下载</a>

嵌入式软件开发离不开开发板，在没有物理开发板的情况下，可以使用 QEMU 等类似的虚拟机来模拟开发板。QEMU 是一个支持跨平台虚拟化的虚拟机，它可以虚拟很多开发板。为了方便大家在没有开发板的情况下体验 RT-Thread，RT-Thread 提供了 QEMU 模拟的 ARM vexpress A9 开发板的板级支持包 (BSP)。
本文主要介绍在 Window 平台上使用 QEMU 运行 RT-Thread qemu-vexpress-a9 BSP 工程，并介绍了如何使用虚拟网卡连接 QEMU 到网络。

## 准备工作

- Git 工具：[下载地址](https://www.git-scm.com/download/)
- tap 网卡：[下载地址](https://pan.baidu.com/s/1h2BmdL9myK6S0g8TlfSW0g)

git 和 tap 网卡一路默认安装即可。

RT-Thread 提供的 QEMU 模拟的 ARM vexpress A9 开发板的板级支持包 (BSP) 位于 RT-Thread 源码 BSP 目录下的 qemu-vexpress-a9 文件夹，此 BSP 实现了 LCD、键盘、鼠标、SD 卡、以太网卡、串口等相关驱动，文件夹内容如下图所示。

![qemu-vexpress-a9 文件夹](figures/qemubsp.png)

qemu-vexpress-a9 BSP 主要文件及目录描述如下所示：

| 文件 / 目录        | 描述               |
| ------------- | --------------- |
| .vscode       | vscode 配置文件   |
| applications  | 用户应用代码目录   |
| cpu           | 芯片相关          |
| drivers       | RT-Thread 提供的底层驱动 |
| qemu.bat      | Windows 平台运行脚本文件 |
| qemu.sh       | Linux 平台运行脚本文件 |
| qemu-dbg.bat  | Windows 平台调试脚本文件 |
| qemu-dbg.sh   | Linux 平台调试脚本文件 |
|  README.md    | BSP 说明文件 |
| rtconfig.h    | BSP 配置头文件 |

## 编译和运行 RT-Thread

### 步骤一 使用 scons 命令编译工程

打开 Env 文件夹，双击 env.exe 文件打开 Env 控制台：

![Env 文件夹](figures/env.png)

在 Env 控制台下切换目录，输入命令 `cd D:\repository\rt-thread\bsp\qemu-vexpress-a9` 切换到 RT-Thread 源码文件夹下的 qemu-vexpress-a9  BSP 根目录，然后输入 `scons` 命令编译工程，如果编译正确无误，会在 BSP 目录下生成 QEMU 下运行的 rtthread.elf 目标文件。

![编译工程](figures/scons.png)

### 步骤二 使用 qemu.bat 命令运行工程

编译完成后输入 `qemu.bat` 启动虚拟机及 BSP 工程，qemu.bat 是 Window 批处理文件，此文件位于 BSP 文件夹下，主要包括 QEMU 的执行指令，第一次运行工程会在 BSP 文件夹下创建一份空白的 sd.bin 文件，这是虚拟的 sd 卡，大小为 64M。Env 命令行界面显示 RT-Thread 系统启动过程中打印的初始化信息及版本号信息等，qemu 虚拟机也运行起来了。如下面图片所示：

![运行工程](figures/qemu.bat.png)

![虚拟机](figures/qemu.png)

> [!NOTE]
> 注：若电脑安装有 360 安全卫士会有警告，请点击允许程序运行。

## 运行 RT-Thread Finsh 控制台

RT-Thread 支持 Finsh，用户可以在命令行模式使用命令操作。输入 `help` 或按 tab 键可以查看所有支持的命令。如下图所示，左边为命令，右边为命令描述。

![查看 Finsh 命令 ](figures/finsh-cmd.png)

如下图所示，比如输入`list_thread`命令可以查看当前运行的线程，以及线程状态和堆栈大小等信息。输入`list_timer`可以查看定时器的状态。

![查看系统线程情况 ](figures/finsh-thread.png)

## 运行 RT-Thread 文件系统

输入 `list_device` 可以查看注册到系统的所有设备。如下面图片所示可以看到虚拟的 sd 卡 “sd0” 设备，接下来我们可以使用 `mkfs sd0` 命令格式化 sd 卡，执行该命令会将 sd 卡格式化成 FatFS 文件系统。FatFs 是专为小型嵌入式设备开发的一个兼容微软 fat 的文件系统，采用 ANSI C 编写，采用抽象的硬件 I/O 层以及提供持续的维护，因此具有良好的硬件无关性以及可移植性。

了解 FatFS 详细信息请点击链接：<http://elm-chan.org/fsw/ff/00index_e.html>

![格式化 sd 卡](figures/mkfs-sd0.png)

第一次格式化 sd 卡后文件系统不会马上装载上，第二次启动才会被正确装载。我们退出虚拟机，然后在 Env 命令行界面输入 `qemu.bat` 重新启动虚拟机及工程，输入 `ls` 命令可以看到新增了 Directory 目录，文件系统已经装载上，然后可以使用 RT-Thread 提供的其他命令体验文件系统

![文件系统其他命令](figures/echo-cat.png)

## 运行 RT-Thread 网络

### 步骤一 配置 TAP 网卡

打开网络和共享中心更改适配器设置，将安装的虚拟网卡重命名为 tap，如下图所示：

 ![重命名 Tap 网卡](figures/tap_rename.png)

右键当前能上网的网络连接（本文使用以太网），打开属性 -> 共享，选择家庭网络连接为 tap，点击确定完成设置，如下图所示：（如果只有一个网卡的话，就不用下拉选择网卡了，只要勾选允许共享即可）

 ![设置网络共享](figures/tap_share_internet.png)

> [!NOTE]
> 注：tap 网卡和 VMware 的虚拟网卡可能会冲突，如果出现无法开启网络共享，或者 ping 不通网络的情况，请禁用 VMware 虚拟网卡之后再尝试一次。

### 步骤二 修改 qemu.bat 脚本文件

1. 打开 qemu-vexpress-a9 BSP 目录下的 qemu.bat 文件。

2. 在下图所示位置添加 `-net nic -net tap,ifname=tap` 配置。其中 ifname=tap 的意思是网卡的名称是 tap。

![修改 qemu 启动参数](figures/qemu_modify.png)

### 步骤三 查看 IP 地址

输入 `qemu.bat` 命令运行工程，在 shell 中输入 `ifconfig`命令查看网络状态，正常获取到 IP 即表示网络驱动正常，配置工作完成，效果如下图所示：

![查看 IP 地址](figures/ifconfig.png)

> [!NOTE]
> 注：* 当出现获取不到 IP 地址的情况时，在 qemu 运行的情况下，先将以太网的共享关闭，然后再次打开即可。
    * 如果获取到的 IP 是 10.0.x,x，是因为没有为 QEMU 添加启动参数  `-net nic -net tap,ifname=tap` 。
    * 虚拟机刚开始运行的时候并不会立刻获取到 IP 地址，有时需要等待几秒钟才会获取到 IP。
    * 关闭虚拟机可以按 Ctrl + 'C' 来结束程序运行。

## 运行 RT-Thread Ping 工具

### 步骤一 下载网络工具软件包

1、在路径`bsp\qemu-vexpress-a9`下打开 Env 工具，执行 menuconfig，如下图所示:

![执行 menuconfig](figures/env_menuconfig.png)

2、在 RT-Thread online packages->IoT - internet of things 页面打开 netutils: Networking utilities for RT-Thread 功能，如下图所示：

 ![开启网络工具包](figures/online_packages.png)

3、进入 netutils: Networking utilities for RT-Thread 页面，打开 Enable Ping utility 功能，如下图所示：

 ![开启 Ping 工具包](figures/enable_ping.png)

4、保存并退出配置界面。如果没有开启 Env 自动更新软件包功能的话，需要输入 `pkgs --update` 更新软件包配置。更新完成后使用 scons 命令重新编译工程，如下图所示：

![scons 编译工程](figures/scons-net.png)

5、编译完成之后运行 qemu.bat 文件，如下图所示：

![运行工程](figures/qemu_bat.png)

### 步骤二 运行 ping 工具

在 shell 中输入 `ifconfig`命令查看网络状态，正常获取到 IP 即表示网络驱动正常：

![运行结果](figures/ifconfig.png)

在 shell 中输入 ping www.rt-thread.com 可以看到 ping 通的返回结果， 表示网络配置成功，能够 ping 通，如下图所示：

![ping 的结果](figures/ping.png)

## 参考资料

* [《Env 用户手册》](../../../programming-manual/env/env.md)
* [文件系统应用笔记](https://www.rt-thread.org/document/site/application-note/components/dfs/an0012-dfs/)

## 常见问题

* [常见问题及解决方法](../faq/faq.md)。
