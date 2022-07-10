# 动态模块 #

在传统桌面操作系统中，用户空间和内核空间是分开的，应用程序运行在用户空间，内核以及内核模块则运行于内核空间，其中内核模块可以动态加载与删除以扩展内核功能。`dlmodule` 则是 RT-Thread 下，在内核空间对外提供的动态模块加载机制的软件组件。在 RT-Thread v3.1.0 以前的版本中，这也称之为应用模块（Application Module），在 RT-Thread v3.1.0 及之后，则回归传统，以动态模块命名。

`dlmodule` 组件更多的是一个 ELF 格式加载器，把单独编译的一个 elf 文件的代码段，数据段加载到内存中，并对其中的符号进行解析，绑定到内核导出的 API 地址上。动态模块 elf 文件主要放置于 RT-Thread 下的文件系统上。

## 功能简介 ##

动态模块为 RT-Thread 提供了动态加载程序模块的机制，因为也独立于内核编译，所以使用方式比较灵活。从实现上讲，这是一种将内核和动态模块分开的机制，通过这种机制，内核和动态模块可以分开编译，并在运行时通过内核中的模块加载器将编译好的动态模块加载到内核中运行。

在 RT-Thread 的动态模块中，目前支持两种格式：

* `.mo` 则是编译出来时以 `.mo` 做为后缀名的可执行动态模块；它可以被加载，并且系统中会自动创建一个主线程执行这个动态模块中的 `main` 函数；同时这个 `main(int argc, char**argv)` 函数也可以接受命令行上的参数。
* `.so` 则是编译出来时以 `.so` 做为后缀名的动态库；它可以被加载，并驻留在内存中，并提供一些函数集由其他程序（内核里的代码或动态模块）来使用。

当前 RT-Thread 支持动态模块的架构主要包括 ARM 类架构和 x86 架构，未来会扩展到 MIPS，以及 RISC-V 等架构上。RT-Thread 内核固件部分可使用多种编译器工具链，如 GCC, ARMCC、IAR 等工具链；但动态模块部分编译当前只支持 GNU GCC 工具链编译。因此编译 RT-Thread 模块需下载 GCC 工具，例如 CodeSourcery 的 arm-none-eabi 工具链。一般的，最好内核和动态模块使用一样的工具链进行编译（这样不会在 libc 上产生不一致的行为）。另外，动态模块一般只能加载到 RAM 中使用，并进行符号解析绑定到内核导出的 API 地址上，而不能基于 Flash 直接以 XIP 方式运行（因为 Flash 上也不能够再行修改其中的代码段）。

## 使用动态模块 ##

当要在系统中测试使用动态模块，需要编译一份支持动态模块的固件，以及需要运行的动态模块。下面将固件和动态模块的编译方式分为两部分进行介绍。

### 编译固件 ###

当要使用动态模块时，需要在固件的配置中打开对应的选项，使用 menuconfig 打开如下配置：

```c
   RT-Thread Components  --->
       POSIX layer and C standard library  --->
           [*] Enable dynamic module with dlopen/dlsym/dlclose feature
```

也要打开文件系统的配置选项：

```c
   RT-Thread Components  --->
        Device virtual file system  --->
               [*] Using device virtual file system
```

bsp 对应的 rtconfig.py 中设置动态模块编译时需要用到的配置参数：

```Python
M_CFLAGS = CFLAGS + ' -mlong-calls -fPIC '
M_CXXFLAGS = CXXFLAGS + ' -mlong-calls -fPIC'
M_LFLAGS = DEVICE + CXXFLAGS + ' -Wl,--gc-sections,-z,max-page-size=0x4' +\
                                ' -shared -fPIC -nostartfiles -nostdlib -static-libgcc'
M_POST_ACTION = STRIP + ' -R .hash $TARGET\n' + SIZE + ' $TARGET \n'
M_BIN_PATH = r'E:\qemu-dev310\fatdisk\root'
```

相关的解释如下：

* M_CFLAGS - 动态模块编译时用到的 C 代码编译参数，一般此处以 PIC 方式进行编译（即代码地址支持浮动方式执行）；
* M_CXXFLAGS - 动态模块编译时用到的 C++ 代码编译参数，参数和上面的 `M_CFLAGS` 类似；
* M_LFLAGS - 动态模块进行链接时的参数。同样是 PIC 方式，并且是按照共享库方式链接（部分链接）；
* M_POST_ACTIOn - 动态模块编译完成后要进行的动作，这里会对 elf 文件进行 strip 下，以减少 elf 文件的大小；
* M_BIN_PATH - 当动态模块编译成功时，对应的动态模块文件是否需要复制到统一的地方；

基本上来说，ARM9、Cortex-A、Cortex-M 系列的这些编译配置参数是一样的。

内核固件也会通过 `RTM(function)` 的方式导出一些函数 API 给动态模块使用，这些导出符号可以在 msh 下通过命令：

`list_symbols`

列出固件中所有导出的符号信息。`dlmodule` 加载器也是把动态模块中需要解析的符号按照这里导出的符号表来进行解析，完成最终的绑定动作。

这段符号表会放在一个专门的，名字是 RTMSymTab 的 section 中，所以对应的固件链接脚本也需要保留这块区域，而不会被链接器优化移除掉。可以在链接脚本中添加对应的信息：

```text
/* section information for modules */
. = ALIGN(4);
__rtmsymtab_start = .;
KEEP(*(RTMSymTab))
__rtmsymtab_end = .;
```

然后在 BSP 工程目录下执行 `scons` 正确无误地生成固件后，在 BSP 工程目录下执行一下命令：

`scons --target=ua -s`

来生成编译动态模块时需要包括的内核头文件搜索路径及全局宏定义。

### 编译动态模块 ###

在 github 上有一份独立仓库： [rtthread-apps](https://github.com/RT-Thread/rtthread-apps) ，这份仓库中放置了一些和动态模块，动态库相关的示例。

其目录结构如下：

| **目录名** | **说明** |
| --- | ---------------- |
| cxx | 演示了如何在动态模块中使用 C++ 进行编程 |
| hello | 最简单的 `hello world` 示例 |
| lib | 动态库的示例 |
| md5 | 为一个文件产生 md5 码 |
| tools | 动态模块编译时需要使用到的 Python/SConscript 脚本 |
| ymodem | 通过串口以 YModem 协议下载一个文件到文件系统上 |

可以把这份 git clone 到本地，然后在命令行下以 scons 工具进行编译，如果是 Windows 平台，推荐使用 RT-Thread/ENV 工具。

进入控制台命令行后，进入到这个 rtthread-apps repo 所在的目录（同样的，请保证这个目录所在全路径不包含空格，中文字符等字符），并设置好两个变量：

* RTT_ROOT - 指向到 RT-Thread 代码的根目录；
* BSP_ROOT - 指向到 BSP 的工程目录；

Windows 下可以使用 (假设使用的 BSP 是 qemu-vexpress-a9)：

```c
set RTT_ROOT=d:\your_rtthread
set BSP_ROOT=d:\your_rtthread\bsp\qemu-vexpress-a9
```

来设置对应的环境变量。然后使用如下命令来编译动态模块，例如 hello 的例子：

`scons --app=hello`

编译成功后，它会在 rtthread-apps/hello 目录下生成 hello.mo 文件。

也可以编译动态库，例如 lib 的例子：

`scons --lib=lib`

编译成功后，它会在 rtthread-apps/lib 目录下生成 lib.so 文件。

我们可以把这些 mo、so 文件放到 RT-Thread 文件系统下。在 msh 下，可以简单的以 `hello` 命令方式执行 `hello.mo` 动态模块：

```c
msh />ls
Directory /:
hello.mo            1368
lib.so              1376
msh />hello
msh />Hello, world
```

调用 hello 后，会执行 hello.mo 里的 main 函数，执行完毕后退出对应的动态模块。其中 `hello/main.c` 的代码如下：

```c
#include <stdio.h>

int main(int argc, char *argv[])
{
    printf("Hello, world\n");

    return 0;
}
```

## RT-Thread 动态模块 API

除了可以通过 msh 直接加载并执行动态模块外，也可以在主程序中使用 RT-Thread 提供的动态模块 API 来加载或卸载动态模块。

### 加载动态模块

```c
struct rt_dlmodule *dlmodule_load(const char* pgname);
```

|**参数**|**描述**|
| ---- | ---- |
| pgname | 动态模块的路径 |
|**返回**| ——                               |
| 动态模块指针     | 成功加载                  |
| RT_NULL | 失败         |

这个函数从文件系统中加载动态模块到内存中，若正确加载返回该模块的指针。这个函数并不会创建一个线程去执行这个动态模块，仅仅把模块加载到内存中，并解析其中的符号地址。

### 执行动态模块

```c
struct rt_dlmodule *dlmodule_exec(const char* pgname, const char* cmd, int cmd_size);
```

|**参数**|**描述**|
| ---- | ---- |
| pgname | 动态模块的路径 |
| cmd | 包括动态模块命令自身的命令行字符串 |
| cmd_size | 命令行字符串大小 |
|**返回**| ——                               |
| 动态模块指针     | 成功运行                 |
| RT_NULL | 失败         |

这个函数根据 `pgname` 路径加载动态模块，并启动一个线程来执行这个动态模块的 `main` 函数，同时 `cmd` 会作为命令行参数传递给动态模块的 `main` 函数入口。

### 退出动态模块

```c
void dlmodule_exit(int ret_code);
```

|**参数**|**描述**|
| ---- | ---- |
| ret_code | 模块的返回参数 |

这个函数由模块运行时调用，它可以设置模块退出的返回值 `ret_code`，然后从模块退出。

### 查找动态模块

```c
struct rt_dlmodule *dlmodule_find(const char *name);
```

|**参数**|**描述**|
| ---- | ---- |
| name | 模块名称 |
|**返回**| ——                 |
| 动态模块指针     | 成功     |
| RT_NULL | 失败         |

这个函数以 `name` 查找系统中是否已经有加载的动态模块。

### 返回动态模块

```c
struct rt_dlmodule *dlmodule_self(void);
```

|**参数**|**描述**|
| ---- | ---- |
|**返回**| ——                 |
| 动态模块指针     | 成功     |
| RT_NULL | 失败         |

这个函数返回调用上下文环境下动态模块的指针。

### 查找符号

```c
rt_uint32_t dlmodule_symbol_find(const char *sym_str);
```

|**参数**|**描述**|
| ---- | ---- |
| sym_str | 符号名称 |
|**返回**| ——                 |
| 符号地址     | 成功     |
| 0 | 失败         |

这个函数根据符号名称返回符号地址。

## 标准 POSIX 动态库 libdl API ##

在 RT-Thread dlmodule 中也支持 POSIX 标准的 libdl API，类似于把一个动态库加载到内存中（并解析其中的一些符号信息），由这份动态库提供对应的函数操作集。libdl API 需要包含的头文件：

`#include <dlfcn.h>`

### 打开动态库

```c
void * dlopen (const char * pathname, int mode);
```

|**参数**|**描述**|
| ---- | ---- |
| pathname | 动态库路径名称 |
| mode | 打开动态库时的模式，在 RT-Thread 中并未使用 |
|**返回**| ——                 |
| 动态库的句柄 (`struct dlmodule` 结构体指针)     | 成功     |
| NULL | 失败         |

这个函数类似 `dlmodule_load` 的功能，会从文件系统上加载动态库，并返回动态库的句柄指针。

### 查找符号

```c
void* dlsym(void *handle, const char *symbol);
```

|**参数**|**描述**|
| ---- | ---- |
| handle | 动态库句柄，`dlopen` 的返回值 |
| symbol | 要返回的符号地址 |
|**返回**| ——                 |
| 符号地址     | 成功     |
| NULL | 失败         |

这个函数在动态库 `handle` 中查找是否存在 `symbol` 的符号，如果存在返回它的地址。

### 关闭动态库

```
int dlclose (void *handle);
```

|**参数**|**描述**|
| ---- | ---- |
| handle | 动态库句柄 |
|**返回**| ——         |
| 0     | 成功     |
| 负数 | 失败         |

这个函数会关闭 `handle` 指向的动态库，从内存中卸载掉。需要注意的是，当动态库关闭后，原来通过 `dlsym` 返回的符号地址将不再可用。如果依然尝试去访问，可能会引起 fault 错误。

## 常见问题

### Env 工具的相关问题请参考 [《Env 用户手册》](../env/env.md)。

### Q: 根据文档不能成功运行动态模块。

**A:** 请更新 RT-Thread 源代码到 3.1.0 及以上版本。

### Q: 使用 scons 命令编译工程，提示 undefined reference to __rtmsymtab_start。

**A:**  请参考 qemu-vexpress-a9  BSP 的 GCC 链接脚本文件 link.lds，在工程的 GCC 链接脚本的 TEXT 段增加以下内容。

```
    /* section information for modules */
    . = ALIGN(4);
    __rtmsymtab_start = .;
    KEEP(*(RTMSymTab))
    __rtmsymtab_end = .;
```
