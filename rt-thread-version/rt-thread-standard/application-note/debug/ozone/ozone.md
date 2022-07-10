# 使用 Ozone 调试 RT-Thread

## 简介

Ozone 是用于嵌入式应用程序的全功能图形调试器。相 Keil 而言， Ozone 对死机重连更加友好，不会复位芯片

## 使用

Ozone 在板子没有死机的情况下，也可以通过以下方法 Attach 上去，并单步调试，类似 Keil 的功能。但是它的栈解析并不如 Keil 的好。Ozone 在调试死机问题时较为有用。

以下以一款 `Cortex-a` 的芯片为例，讲解 Ozone 的基本使用

### 创建工程

打开软件，选择 Create New Project 创建一个新的工程

![](https://img-blog.csdnimg.cn/img_convert/2aefb6218c9110f98073b8d967dd4f87.png)

这里也可以打开一个已经存在的工程 Open Exiting Project，本文以新建工程为例

### 选择目标器件

选择目标器件是，主要选择的是处理器内核的架构，如 Cortex-M3、Cortex-M4 和 Cortex-A 等，这里以 Cortex-A5 为例

![](https://img-blog.csdnimg.cn/img_convert/5e4ced90f264d248e1e8c6920a33ffc3.png)

上图中外设文件是可选的，该文件一般由具体的半导体厂商提供，来查看具体的外设寄存器信息，如果没有是不影响 C 代码调试的

### 选择通信方式

这里需要根据板子的具体通信方式来选择，例如：JTAG，SWD 等，本文使用的是 JTAG，速度使用默认的即可

![](https://img-blog.csdnimg.cn/img_convert/f7bbbf2d93812df6bc25f1fbbd061285.png)

### 选择目标文件，读取符号信息

这里选择的是通过 RT-Thread Studio/GCC/MDK 等 IDE 工具生成的 `elf` 文件，elf 文件中包含所有的符号和地址信息，所以调试的时候不需要源码，只需要加载 elf 文件即可，当在 elf 文件中定位到问题之后，再修改对应的源码即可

![](https://img-blog.csdnimg.cn/img_convert/e8a55b8437beafd7045a4c5fe1d57037.png)

### 其它可选的设置

这里的设置如果没有特殊的配置，默认即可

![](https://img-blog.csdnimg.cn/img_convert/511cc63b1da04d512a169fd84bb69129.png)

### 连接目标芯片

上述步骤完成之后，工程就建立完毕，接下来就可以后将 Ozone 通过 `link Attach` 到死机的板子并且 Halt 住板子

![](https://img-blog.csdnimg.cn/img_convert/3516ad3317a3296558712d2235d933d9.png)

如果程序是由于异常导致的死机，那么当 Ozone 连上的时候，可以看到死机的现场(如果使用其它的 IDE 这个时候很可能会复位芯片，无法保留现场)

### 查看函数调用栈

打开 Call Stack 窗口，查看函数调用栈

![](https://img-blog.csdnimg.cn/img_convert/2f24249dff7f516879946ea7f6d40eec.png)

### 查看变量

![](https://img-blog.csdnimg.cn/img_convert/18884cf95b516eed22512acdc7f1c084.png)

### 查看汇编

打开 Disassemble 窗口，查看汇编代码

![](https://img-blog.csdnimg.cn/img_convert/30f8109c708f144713b50b68cf60507f.png)

### 查看寄存器

打开 Registers 窗口，查看寄存器，这里需要注意的是，如果没有选择外设的描述文件，那么这里只能看到内核相关的寄存器，而看不到外设寄存器

![](https://img-blog.csdnimg.cn/img_convert/8498c51f38649b471688c8308cfa02d8.png)

### 查看内存

打开 Memory 窗口，查看内存

![](https://img-blog.csdnimg.cn/img_convert/885e93ce3205b3997b4e55969d76851e.png)

这里只介绍了 Ozone 的一些基本用法，更多用法参见 [Ozone 官方文档](https://www.segger.com/products/development-tools/ozone-j-link-debugger/) 
