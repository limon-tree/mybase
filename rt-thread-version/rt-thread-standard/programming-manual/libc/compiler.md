# 编译器配平层

编译器配平层负责对接编译器的底层桩函数以及平衡不同编译器内置C库函数的差异而设计，为上层 POSIX 层提供功能统一的接口，位于 [components/libc/compiler](https://github.com/RT-Thread/rt-thread/tree/master/components/libc/compilers) 文件目录下。之所以需要配平，是由于 GCC (newlib)、Keil-MDK、IAR、Visual Studio (WIN32) 四个编译工具链内置 LIBC 所提供的标准 C 库函数支持程度不一样，编译器配平层负责将四种不同的编译链所提供的标准 C 库平衡到相同的水平。编译器配平层无需用户手动干预，会在编译工程时，根据用户所使用的的编译平台和工具链自动进行配平。保证上层无需区分使用何种编译平台和工具链的 LIBC ，使用通用的头文件即可引用 LIBC 的相关函数。

配平层已经提供了一些基本的 LIBC 函数，这些函数不需要用户额外使能，可以即刻使用。

## 标准输出函数 (printf 家族)

### printf 函数

在 Keil 和 IAR 编译平台下，用户可以直接使用 printf 函数；在 GCC 下，需要额外使能 `RT_USING_POSIX_FS` 和 `RT_USING_POSIX_STDIO` 宏才能使用 printf 函数。

### rt_kprintf 函数 与 printf 函数的使用选择

如果不是特殊需求，建议使用 rt_kprintf 函数，因为 printf 是由编译平台内部提供的，其空间占用、以及内存使用情况我们无从得知，printf 函数要比 rt_kprintf 函数的 ROM 占用大很多。

无论是 rt_kprintf 函数以及 printf 函数都是非线程函数，在多线程同时使用的情况下，会出现交叉打印的现象，该问题是正常现象，因为根据 C 标准的要求，printf 函数就是非线程安全的。

原生 rt_kprintf 函数是经过优化的，占用空间要比 printf 函数小很多。但是 rt_kprintf 函数并不支持浮点类型的输出。因此：

1. 如需 rt_kprintf 函数支持浮点类型的输出，可以安装 [rt_vsnprintf_full 软件包](https://github.com/mysterywolf/rt_vsnprintf_full)。
2. 如需 rt_kprintf 函数支持线程安全输出，可以安装 [rt_kprintf_threadsafe 软件包](https://github.com/mysterywolf/rt_kprintf_threadsafe)。
3. 上述两个软件包可以同时安装使用，以同时让 rt_kprintf 支持线程安全和浮点类型输出的能力。

### 其他字符串格式化输出函数 (例如 snprintf 等)

其他字符串格式化输出函数（例如 snprintf 等）都是可以直接使用的。

强烈建议用户使用 rt_snprintf 等 RT-Thread 这侧的函数来代替 snprintf 等函数以降低资源消耗。尤其是在 GCC 编译链下，Newlib（GCC 工具链内部默认的 C 库）内置提供的 snprintf 函数是非线程安全的，在多线程无保护输出浮点数的情况下可能会引发死机（snprintf 函数非线程安全是不正常的）。

同理，原生 rt_snprintf 等函数不支持浮点输出，用户可以通过安装 [rt_vsnprintf_full 软件包](https://github.com/mysterywolf/rt_vsnprintf_full) 来支持浮点数输出。

## 内存相关函数

malloc 、realloc 、calloc 、free 内存分配函数可以直接使用，其已经被 rt_malloc 、rt_calloc 、rt_realloc 、rt_free 函数接管，二者无任何区别。

memcpy 、memset 等内存操作相关函数。这里需要注意的时，建议使用 rt_memcpy 以及 rt_memset 来代替 memcpy 和 memset ，虽然 rt_memxxx 函数要比C库的内存操作函数要慢一些，但是要更安全；除非您对拷贝速度非常在意，否则建议使用 rt_memxxx 函数。如果使用的是 Cortex-M 的 MCU，可以安装 [rt_memcpy_cm](https://github.com/mysterywolf/rt_memcpy_cm) 软件包来对 rt_memcpy 函数进行汇编加速，该软件包安全且高效。

## 字符串相关函数

strlen 等字符串相关函数为编译器C库内置函数，可以直接使用。

## 系统函数

exit 、abort 函数，调用后相当于删除当前线程。

system 函数可以用于在 C 程序中内置执行一些 FinSH 命令。

## 时间函数

所有 ANSI/ISO-C 规定的时间函数均可以使用，例如 time 、ctime 等函数。注意：部分涉及到获取当前时间的函数需要使能 RTC 设备才可以正常工作，如果没有使能 RTC 设备就调用这些函数，会在串口终端给出警告。

支持时区功能，可以通过 `RT_LIBC_DEFAULT_TIMEZONE` 宏定义设置默认时区，默认的时区为 UTC+8 北京时间，在运行过程中也可以使用 tz_set() 、 tz_get() 、 tz_is_dst() 函数动态设置或获取时区相关信息（需要包含 <sys/time.h> 头文件）。

***注意：***

1. 时区是给人看的，不是给机器看的。因此，底层驱动不应当使用带有时区的时间，而应该使用格林威治时间，即 UTC+0。当且仅当该时间需要显示出来给人看的时候，才需要临时将其使用带有时区转换功能的函数（例如 ctime）显示时间。否则很容易引发时间上的错乱。

   注意以下两组函数的区别，功能相同，唯一的区别是是否考虑时区问题：

   |        | 考虑时区  | 不考虑时区 |
   | ------ | --------- | ---------- |
   | 第一组 | localtime | gmtime     |
   | 第二组 | mktime    | timegm     |

2. 目前，时间相关函数不支持的是夏令时自动转换相关功能。夏令时在中国不使用，主要是欧美国家在使用。

## kservice.c

在RT-Thread内核 (Kernel) 中有一个叫 kservice.c 的文件。该文件提供了RT-Thread自己实现的C库函数，例如 rt_strlen、rt_strcpy 等等。这些函数存在的意义在于让 RT-Thread 内核具备自持能力，即在不依赖任何C库的情况下，依然可以正常运行。此外，部分编译器内置的C库函数，如上所说，存在一些缺陷，kservice.c 文件中提供的函数更安全，且可控。如果认为，kservice.c 文件中提供的函数与编译器内置C库函数重复，可以通过配置，将 kservice.c 中的大多数函数重映射到编译器内置C库函数中，例如：在开启如下图所示的选项后，调用 rt_strlen 函数实际相当于直接调用编译器内置C库的 strlen 函数。

![kservice](figures/kservice.png)
