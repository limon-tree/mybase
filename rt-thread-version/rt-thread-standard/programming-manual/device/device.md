# I/O 设备模型

绝大部分的嵌入式系统都包括一些 I/O（Input/Output，输入 / 输出）设备，例如仪器上的数据显示屏、工业设备上的串口通信、数据采集设备上用于保存数据的 Flash 或 SD 卡，以及网络设备的以太网接口等，都是嵌入式系统中容易找到的 I/O 设备例子。

本章主要介绍 RT-Thread 如何对不同的 I/O 设备进行管理，读完本章，我们会了解 RT-Thread 的 I/O 设备模型，并熟悉 I/O 设备管理接口的不同功能。

## I/O 设备介绍

### I/O 设备模型框架

RT-Thread 提供了一套简单的 I/O 设备模型框架，如下图所示，它位于硬件和应用程序之间，共分成三层，从上到下分别是 I/O 设备管理层、设备驱动框架层、设备驱动层。

![I/O 设备模型框架](figures/io-dev.png)

应用程序通过 I/O 设备管理接口获得正确的设备驱动，然后通过这个设备驱动与底层 I/O 硬件设备进行数据（或控制）交互。

I/O 设备管理层实现了对设备驱动程序的封装。应用程序通过 I/O 设备层提供的标准接口访问底层设备，设备驱动程序的升级、更替不会对上层应用产生影响。这种方式使得设备的硬件操作相关的代码能够独立于应用程序而存在，双方只需关注各自的功能实现，从而降低了代码的耦合性、复杂性，提高了系统的可靠性。

设备驱动框架层是对同类硬件设备驱动的抽象，将不同厂家的同类硬件设备驱动中相同的部分抽取出来，将不同部分留出接口，由驱动程序实现。

设备驱动层是一组驱使硬件设备工作的程序，实现访问硬件设备的功能。它负责创建和注册 I/O 设备，对于操作逻辑简单的设备，可以不经过设备驱动框架层，直接将设备注册到 I/O 设备管理器中，使用序列图如下图所示，主要有以下 2 点：

* 设备驱动根据设备模型定义，创建出具备硬件访问能力的设备实例，将该设备通过 `rt_device_register()` 接口注册到 I/O 设备管理器中。

* 应用程序通过 `rt_device_find()` 接口查找到设备，然后使用 I/O 设备管理接口来访问硬件。

![简单 I/O 设备使用序列图](figures/io-call.png)

对于另一些设备，如看门狗等，则会将创建的设备实例先注册到对应的设备驱动框架中，再由设备驱动框架向 I/O 设备管理器进行注册，主要有以下几点：

* 看门狗设备驱动程序根据看门狗设备模型定义，创建出具备硬件访问能力的看门狗设备实例，并将该看门狗设备通过 `rt_hw_watchdog_register()` 接口注册到看门狗设备驱动框架中。

* 看门狗设备驱动框架通过 `rt_device_register()` 接口将看门狗设备注册到 I/O 设备管理器中。

* 应用程序通过 I/O 设备管理接口来访问看门狗设备硬件。

看门狗设备使用序列图:

![看门狗设备使用序列图](figures/wtd-uml.png)

### I/O 设备模型

RT-Thread 的设备模型是建立在内核对象模型基础之上的，设备被认为是一类对象，被纳入对象管理器的范畴。每个设备对象都是由基对象派生而来，每个具体设备都可以继承其父类对象的属性，并派生出其私有属性，下图是设备对象的继承和派生关系示意图。

![设备继承关系图](figures/io-parent.png)

设备对象具体定义如下所示：

```c
struct rt_device
{
    struct rt_object          parent;        /* 内核对象基类 */
    enum rt_device_class_type type;          /* 设备类型 */
    rt_uint16_t               flag;          /* 设备参数 */
    rt_uint16_t               open_flag;     /* 设备打开标志 */
    rt_uint8_t                ref_count;     /* 设备被引用次数 */
    rt_uint8_t                device_id;     /* 设备 ID,0 - 255 */

    /* 数据收发回调函数 */
    rt_err_t (*rx_indicate)(rt_device_t dev, rt_size_t size);
    rt_err_t (*tx_complete)(rt_device_t dev, void *buffer);

    const struct rt_device_ops *ops;    /* 设备操作方法 */

    /* 设备的私有数据 */
    void *user_data;
};
typedef struct rt_device *rt_device_t;

```

### I/O 设备类型

RT-Thread 支持多种 I/O 设备类型，主要设备类型如下所示：

```c
RT_Device_Class_Char             /* 字符设备       */
RT_Device_Class_Block            /* 块设备         */
RT_Device_Class_NetIf            /* 网络接口设备    */
RT_Device_Class_MTD              /* 内存设备       */
RT_Device_Class_RTC              /* RTC 设备        */
RT_Device_Class_Sound            /* 声音设备        */
RT_Device_Class_Graphic          /* 图形设备        */
RT_Device_Class_I2CBUS           /* I2C 总线设备     */
RT_Device_Class_USBDevice        /* USB device 设备  */
RT_Device_Class_USBHost          /* USB host 设备   */
RT_Device_Class_SPIBUS           /* SPI 总线设备     */
RT_Device_Class_SPIDevice        /* SPI 设备        */
RT_Device_Class_SDIO             /* SDIO 设备       */
RT_Device_Class_Miscellaneous    /* 杂类设备        */
```

其中字符设备、块设备是常用的设备类型，它们的分类依据是设备数据与系统之间的传输处理方式。字符模式设备允许非结构的数据传输，即通常数据传输采用串行的形式，每次一个字节。字符设备通常是一些简单设备，如串口、按键。

块设备每次传输一个数据块，例如每次传输 512 个字节数据。这个数据块是硬件强制性的，数据块可能使用某类数据接口或某些强制性的传输协议，否则就可能发生错误。因此，有时块设备驱动程序对读或写操作必须执行附加的工作，如下图所示：

![块设备](figures/block-dev.png)

当系统服务于一个具有大量数据的写操作时，设备驱动程序必须首先将数据划分为多个包，每个包采用设备指定的数据尺寸。而在实际过程中，最后一部分数据尺寸有可能小于正常的设备块尺寸。如上图中每个块使用单独的写请求写入到设备中，头 3 个直接进行写操作。但最后一个数据块尺寸小于设备块尺寸，设备驱动程序必须使用不同于前 3 个块的方式处理最后的数据块。通常情况下，设备驱动程序需要首先执行相对应的设备块的读操作，然后把写入数据覆盖到读出数据上，然后再把这个 “合成” 的数据块作为一整个块写回到设备中。例如上图中的块 4，驱动程序需要先把块 4 所对应的设备块读出来，然后将需要写入的数据覆盖至从设备块读出的数据上，使其合并成一个新的块，最后再写回到块设备中。

## 创建和注册 I/O 设备

驱动层负责创建设备实例，并注册到 I/O 设备管理器中，可以通过静态申明的方式创建设备实例，也可以用下面的接口进行动态创建：

```c
rt_device_t rt_device_create(int type, int attach_size);
```

|**参数**  |**描述**                          |
|-------------|-------------------------------------|
| type        | 设备类型，可取前面小节列出的设备类型值 |
| attach_size | 用户数据大小                        |
|**返回**  | ——                                  |
| 设备句柄    | 创建成功                            |
| RT_NULL     | 创建失败，动态内存分配失败          |

调用该接口时，系统会从动态堆内存中分配一个设备控制块，大小为 struct rt_device 和 attach_size 的和，设备的类型由参数 type 设定。设备被创建后，需要实现它访问硬件的操作方法。

```c
struct rt_device_ops
{
    /* common device interface */
    rt_err_t  (*init)   (rt_device_t dev);
    rt_err_t  (*open)   (rt_device_t dev, rt_uint16_t oflag);
    rt_err_t  (*close)  (rt_device_t dev);
    rt_size_t (*read)   (rt_device_t dev, rt_off_t pos, void *buffer, rt_size_t size);
    rt_size_t (*write)  (rt_device_t dev, rt_off_t pos, const void *buffer, rt_size_t size);
    rt_err_t  (*control)(rt_device_t dev, int cmd, void *args);
};

```

各个操作方法的描述如下表所示：

|**方法名称**|**方法描述**       |
|----|-----------------------|
| init   | 初始化设备。设备初始化完成后，设备控制块的 flag 会被置成已激活状态 (RT_DEVICE_FLAG_ACTIVATED)。如果设备控制块中的 flag 标志已经设置成激活状态，那么再运行初始化接口时会立刻返回，而不会重新进行初始化。              |
| open   | 打开设备。有些设备并不是系统一启动就已经打开开始运行，或者设备需要进行数据收发，但如果上层应用还未准备好，设备也不应默认已经使能并开始接收数据。所以建议在写底层驱动程序时，在调用 open 接口时才使能设备。                 |
| close  | 关闭设备。在打开设备时，设备控制块会维护一个打开计数，在打开设备时进行 + 1 操作，在关闭设备时进行 - 1 操作，当计数器变为 0 时，才会进行真正的关闭操作。|
| read    | 从设备读取数据。参数 pos 是读取数据的偏移量，但是有些设备并不一定需要指定偏移量，例如串口设备，设备驱动应忽略这个参数。而对于块设备来说，pos 以及 size 都是以块设备的数据块大小为单位的。例如块设备的数据块大小是 512，而参数中 pos = 10, size = 2，那么驱动应该返回设备中第 10 个块 （从第 0 个块做为起始），共计 2 个块的数据。这个接口返回的类型是 rt_size_t，即读到的字节数或块数目。正常情况下应该会返回参数中 size 的数值，如果返回零请设置对应的 errno 值。 |
| write   | 向设备写入数据。参数 pos 是写入数据的偏移量。与读操作类似，对于块设备来说，pos 以及 size 都是以块设备的数据块大小为单位的。这个接口返回的类型是 rt_size_t，即真实写入数据的字节数或块数目。正常情况下应该会返回参数中 size 的数值，如果返回零请设置对应的 errno 值。  |
| control | 根据 cmd 命令控制设备。命令往往是由底层各类设备驱动自定义实现。例如参数 RT_DEVICE_CTRL_BLK_GETGEOME，意思是获取块设备的大小信息。 |

当一个动态创建的设备不再需要使用时可以通过如下函数来销毁：

```c
void rt_device_destroy(rt_device_t device);
```

|**参数**|**描述**|
|----------|----------|
| device   | 设备句柄 |
|**返回**| 无       |

设备被创建后，需要注册到 I/O 设备管理器中，应用程序才能够访问，注册设备的函数如下所示：

```c
rt_err_t rt_device_register(rt_device_t dev, const char* name, rt_uint8_t flags);
```

|**参数** |**描述**   |
|------------|-----------------------|
| dev        | 设备句柄              |
| name       | 设备名称，设备名称的最大长度由 rtconfig.h 中定义的宏 RT_NAME_MAX 指定，多余部分会被自动截掉 |
| flags      | 设备模式标志 |
|**返回** | ——                               |
| RT_EOK     | 注册成功                  |
| -RT_ERROR | 注册失败，dev 为空或者 name 已经存在         |

> [!NOTE]
> 注：应当避免重复注册已经注册的设备，以及注册相同名字的设备。

flags 参数支持下列参数 (可以采用或的方式支持多种参数)：

```c
#define RT_DEVICE_FLAG_RDONLY       0x001 /* 只读 */
#define RT_DEVICE_FLAG_WRONLY       0x002 /* 只写  */
#define RT_DEVICE_FLAG_RDWR         0x003 /* 读写  */
#define RT_DEVICE_FLAG_REMOVABLE    0x004 /* 可移除  */
#define RT_DEVICE_FLAG_STANDALONE   0x008 /* 独立   */
#define RT_DEVICE_FLAG_SUSPENDED    0x020 /* 挂起  */
#define RT_DEVICE_FLAG_STREAM       0x040 /* 流模式  */
#define RT_DEVICE_FLAG_INT_RX       0x100 /* 中断接收 */
#define RT_DEVICE_FLAG_DMA_RX       0x200 /* DMA 接收 */
#define RT_DEVICE_FLAG_INT_TX       0x400 /* 中断发送 */
#define RT_DEVICE_FLAG_DMA_TX       0x800 /* DMA 发送 */
```

设备流模式 RT_DEVICE_FLAG_STREAM 参数用于向串口终端输出字符串：当输出的字符是 `“\n”` 时，自动在前面补一个 `“\r”` 做分行。

注册成功的设备可以在 FinSH 命令行使用 `list_device` 命令查看系统中所有的设备信息，包括设备名称、设备类型和设备被打开次数：

```c
msh />list_device
device           type         ref count
-------- -------------------- ----------
e0       Network Interface    0
sd0      Block Device         1
rtc      RTC                  0
uart1    Character Device     0
uart0    Character Device     2
msh />
```

当设备注销后的，设备将从设备管理器中移除，也就不能再通过设备查找搜索到该设备。注销设备不会释放设备控制块占用的内存。注销设备的函数如下所示：

```c
rt_err_t rt_device_unregister(rt_device_t dev);
```

|**参数**|**描述**|
|----------|----------|
| dev      | 设备句柄 |
|**返回**| ——       |
| RT_EOK   | 成功     |

下面代码为看门狗设备的注册示例，调用 `rt_hw_watchdog_register()` 接口后，设备通过 `rt_device_register()` 接口被注册到 I/O 设备管理器中。

```c
const static struct rt_device_ops wdt_ops =
{
    rt_watchdog_init,
    rt_watchdog_open,
    rt_watchdog_close,
    RT_NULL,
    RT_NULL,
    rt_watchdog_control,
};

rt_err_t rt_hw_watchdog_register(struct rt_watchdog_device *wtd,
                                 const char                *name,
                                 rt_uint32_t                flag,
                                 void                      *data)
{
    struct rt_device *device;
    RT_ASSERT(wtd != RT_NULL);

    device = &(wtd->parent);

    device->type        = RT_Device_Class_Miscellaneous;
    device->rx_indicate = RT_NULL;
    device->tx_complete = RT_NULL;

    device->ops         = &wdt_ops;
    device->user_data   = data;

    /* register a character device */
    return rt_device_register(device, name, flag);
}

```

## 访问 I/O 设备

应用程序通过 I/O 设备管理接口来访问硬件设备，当设备驱动实现后，应用程序就可以访问该硬件。I/O 设备管理接口与 I/O 设备的操作方法的映射关系下图所示：

![I/O 设备管理接口与 I/O 设备的操作方法的映射关系](figures/io-fun-call.png)

### 查找设备

应用程序根据设备名称获取设备句柄，进而可以操作设备。查找设备函数如下所示：

```c
rt_device_t rt_device_find(const char* name);
```

|**参数**|**描述**                         |
|----------|------------------------------------|
| name     | 设备名称                           |
|**返回**| ——                                 |
| 设备句柄 | 查找到对应设备将返回相应的设备句柄 |
| RT_NULL  | 没有找到相应的设备对象             |

### 初始化设备

获得设备句柄后，应用程序可使用如下函数对设备进行初始化操作：

```c
rt_err_t rt_device_init(rt_device_t dev);
```

|**参数**|**描述**     |
|----------|----------------|
| dev      | 设备句柄       |
|**返回**| ——             |
| RT_EOK   | 设备初始化成功 |
| 错误码   | 设备初始化失败 |

> [!NOTE]
> 注：当一个设备已经初始化成功后，调用这个接口将不再重复做初始化 0。

### 打开和关闭设备

通过设备句柄，应用程序可以打开和关闭设备，打开设备时，会检测设备是否已经初始化，没有初始化则会默认调用初始化接口初始化设备。通过如下函数打开设备：

```c
rt_err_t rt_device_open(rt_device_t dev, rt_uint16_t oflags);
```

|**参数** |**描述**                          |
|------------|-----------------------------|
| dev        | 设备句柄                           |
| oflags     | 设备打开模式标志  |
|**返回** | ——                           |
| RT_EOK     | 设备打开成功                           |
|-RT_EBUSY | 如果设备注册时指定的参数中包括 RT_DEVICE_FLAG_STANDALONE 参数，此设备将不允许重复打开 |
| 其他错误码 | 设备打开失败                         |

oflags 支持以下的参数：

```c
#define RT_DEVICE_OFLAG_CLOSE 0x000   /* 设备已经关闭（内部使用）*/
#define RT_DEVICE_OFLAG_RDONLY 0x001  /* 以只读方式打开设备 */
#define RT_DEVICE_OFLAG_WRONLY 0x002  /* 以只写方式打开设备 */
#define RT_DEVICE_OFLAG_RDWR 0x003    /* 以读写方式打开设备 */
#define RT_DEVICE_OFLAG_OPEN 0x008    /* 设备已经打开（内部使用）*/
#define RT_DEVICE_FLAG_STREAM 0x040   /* 设备以流模式打开 */
#define RT_DEVICE_FLAG_INT_RX 0x100   /* 设备以中断接收模式打开 */
#define RT_DEVICE_FLAG_DMA_RX 0x200   /* 设备以 DMA 接收模式打开 */
#define RT_DEVICE_FLAG_INT_TX 0x400   /* 设备以中断发送模式打开 */
#define RT_DEVICE_FLAG_DMA_TX 0x800   /* 设备以 DMA 发送模式打开 */
```

> [!NOTE]
> 注：如果上层应用程序需要设置设备的接收回调函数，则必须以 RT_DEVICE_FLAG_INT_RX 或者 RT_DEVICE_FLAG_DMA_RX 的方式打开设备，否则不会回调函数。

应用程序打开设备完成读写等操作后，如果不需要再对设备进行操作则可以关闭设备，通过如下函数完成：

```c
rt_err_t rt_device_close(rt_device_t dev);
```

|**参数** |**描述**                         |
|------------|------------------------------------|
| dev        | 设备句柄                           |
|**返回** | ——                                 |
| RT_EOK     | 关闭设备成功                       |
| \-RT_ERROR | 设备已经完全关闭，不能重复关闭设备 |
| 其他错误码 | 关闭设备失败                       |

> [!NOTE]
> 注：关闭设备接口和打开设备接口需配对使用，打开一次设备对应要关闭一次设备，这样设备才会被完全关闭，否则设备仍处于未关闭状态。

### 控制设备

通过命令控制字，应用程序也可以对设备进行控制，通过如下函数完成：

```c
rt_err_t rt_device_control(rt_device_t dev, rt_uint8_t cmd, void* arg);
```

|**参数**  |**描述**                                 |
|-------------|--------------------------------------------|
| dev         | 设备句柄                                   |
| cmd         | 命令控制字，这个参数通常与设备驱动程序相关 |
| arg         | 控制的参数                                 |
|**返回**  | ——                                         |
| RT_EOK      | 函数执行成功                               |
| -RT_ENOSYS | 执行失败，dev 为空                          |
| 其他错误码  | 执行失败                                   |

参数 cmd 的通用设备命令可取如下宏定义：

```c
#define RT_DEVICE_CTRL_RESUME           0x01   /* 恢复设备 */
#define RT_DEVICE_CTRL_SUSPEND          0x02   /* 挂起设备 */
#define RT_DEVICE_CTRL_CONFIG           0x03   /* 配置设备 */
#define RT_DEVICE_CTRL_SET_INT          0x10   /* 设置中断 */
#define RT_DEVICE_CTRL_CLR_INT          0x11   /* 清中断 */
#define RT_DEVICE_CTRL_GET_INT          0x12   /* 获取中断状态 */
```

### 读写设备

应用程序从设备中读取数据可以通过如下函数完成：

```c
rt_size_t rt_device_read(rt_device_t dev, rt_off_t pos,void* buffer, rt_size_t size);
```

|**参数**         |**描述**                      |
|--------------------|--------------------------------|
| dev                | 设备句柄                       |
| pos                | 读取数据偏移量                     |
| buffer             | 内存缓冲区指针，读取的数据将会被保存在缓冲区中     |
| size               | 读取数据的大小                     |
|**返回**         | ——                        |
| 读到数据的实际大小 | 如果是字符设备，返回大小以字节为单位，如果是块设备，返回的大小以块为单位 |
| 0       | 需要读取当前线程的 errno 来判断错误状态       |

调用这个函数，会从 dev 设备中读取数据，并存放在 buffer 缓冲区中，这个缓冲区的最大长度是 size，pos 根据不同的设备类别有不同的意义。

向设备中写入数据，可以通过如下函数完成：

```c
rt_size_t rt_device_write(rt_device_t dev, rt_off_t pos,const void* buffer, rt_size_t size);
```

|**参数**         |**描述**                      |
|--------------------|--------------------------------|
| dev                | 设备句柄                        |
| pos                | 写入数据偏移量                      |
| buffer             | 内存缓冲区指针，放置要写入的数据           |
| size               | 写入数据的大小         |
|**返回**         | ——                        |
| 写入数据的实际大小 | 如果是字符设备，返回大小以字节为单位；如果是块设备，返回的大小以块为单位 |
| 0       | 需要读取当前线程的 errno 来判断错误状态                  |

调用这个函数，会把缓冲区 buffer 中的数据写入到设备 dev 中，写入数据的最大长度是 size，pos 根据不同的设备类别存在不同的意义。

### 数据收发回调

当硬件设备收到数据时，可以通过如下函数回调另一个函数来设置数据接收指示，通知上层应用线程有数据到达：

```c
rt_err_t rt_device_set_rx_indicate(rt_device_t dev, rt_err_t (*rx_ind)(rt_device_t dev,rt_size_t size));

```

|**参数**|**描述**   |
|----------|--------------|
| dev      | 设备句柄     |
| rx_ind   | 回调函数指针 |
|**返回**| ——           |
| RT_EOK   | 设置成功     |

该函数的回调函数由调用者提供。当硬件设备接收到数据时，会回调这个函数并把收到的数据长度放在 size 参数中传递给上层应用。上层应用线程应在收到指示后，立刻从设备中读取数据。

在应用程序调用 `rt_device_write()` 写入数据时，如果底层硬件能够支持自动发送，那么上层应用可以设置一个回调函数。这个回调函数会在底层硬件数据发送完成后 (例如 DMA 传送完成或 FIFO 已经写入完毕产生完成中断时) 调用。可以通过如下函数设置设备发送完成指示，函数参数及返回值见：

```c
rt_err_t rt_device_set_tx_complete(rt_device_t dev, rt_err_t (*tx_done)(rt_device_t dev,void *buffer));
```

|**参数**|**描述**   |
|----------|--------------|
| dev      | 设备句柄     |
| tx_done  | 回调函数指针 |
|**返回**| ——           |
| RT_EOK   | 设置成功     |

调用这个函数时，回调函数由调用者提供，当硬件设备发送完数据时，由驱动程序回调这个函数并把发送完成的数据块地址 buffer 作为参数传递给上层应用。上层应用（线程）在收到指示时会根据发送 buffer 的情况，释放 buffer 内存块或将其作为下一个写数据的缓存。

### 设备访问示例

下面代码为用程序访问设备的示例，首先通过 `rt_device_find()` 口查找到看门狗设备，获得设备句柄，然后通过 `rt_device_init()` 口初始化设备，通过 `rt_device_control()` 口设置看门狗设备溢出时间。

```c
#include <rtthread.h>
#include <rtdevice.h>

#define IWDG_DEVICE_NAME    "iwg"

static rt_device_t wdg_dev;

static void idle_hook(void)
{
    /* 在空闲线程的回调函数里喂狗 */
    rt_device_control(wdg_dev, RT_DEVICE_CTRL_WDT_KEEPALIVE, NULL);
    rt_kprintf("feed the dog!\n ");
}

int main(void)
{
    rt_err_t res = RT_EOK;
    rt_uint32_t timeout = 1000;    /* 溢出时间 */

    /* 根据设备名称查找看门狗设备，获取设备句柄 */
    wdg_dev = rt_device_find(IWDG_DEVICE_NAME);
    if (!wdg_dev)
    {
        rt_kprintf("find %s failed!\n", IWDG_DEVICE_NAME);
        return RT_ERROR;
    }
    /* 初始化设备 */
    res = rt_device_init(wdg_dev);
    if (res != RT_EOK)
    {
        rt_kprintf("initialize %s failed!\n", IWDG_DEVICE_NAME);
        return res;
    }
    /* 设置看门狗溢出时间 */
    res = rt_device_control(wdg_dev, RT_DEVICE_CTRL_WDT_SET_TIMEOUT, &timeout);
    if (res != RT_EOK)
    {
        rt_kprintf("set %s timeout failed!\n", IWDG_DEVICE_NAME);
        return res;
    }
    /* 设置空闲线程回调函数 */
    rt_thread_idle_sethook(idle_hook);

    return res;
}
```



## 补充说明

### I/O 设备模型框架补充图

 I/O 设备模型框架补充图是对 I/O 设备模型框架图的解释和补充说明，如下图所示。

![I/O 设备模型框架补充图](figures/io-dev-ex.png)



图中各类里的c文件是各类对应的管理接口所在，比如设备基类rt_device的管理接口在device.c中。

图中设备驱动框架层有很多 RT-Thread 写好的类，图中只列出2类，其他类用  “xxx” 来表示，这些省略的类及其管理接口可以在 RT-Thread 源码 components/drivers 目录下找寻，比如该目录下可以找到serial/i2c/spi/sensor/can 等等相关目录。

图中设备驱动层的 “xxx” ，是 RT-Thread 支持的各 BSP 平台，在源码的 src/bsp 目录下找寻，比如stm32/gd32/at32/avr32/k210 等等。各个平台各自实现各个设备类型的硬件驱动能力，比如 STM32分别实现了 stm32_uart 类/stm32_adc 类等及其对应的管理接口；同样的，其他平台也分别各自实现了诸多对应类别及管理接口。

图中设备驱动层的各类里的c文件路径和名字只是示意，具体名字和路径由各BSP开发维护者自己定的。且随着 RT-Thread 版本的变化，各 BSP 的驱动路径和名字可能会发生变化。比如图中画的 STM32 串口设备类的管理接口所在路径是 bsp/stm32/drv_usart.c ，但实际路径是在 RT-Thread 源码下的 bsp/stm32/libraries/HAL_Drivers 下 。比如有的名字叫 drv_uart.c 等等。

该图横向看是分层思想，纵向看是各类派生继承关系。从下到上不断抽象、屏蔽下层差异，体现了面向对象的抽象的思想。子类受到父类的接口约束，子类各自实现父类提供的统一接口，又体现了面向接口编程的思想。比如从驱动层到驱动框架层，由不同厂商的相同硬件模块创建了很多子类对象，然后对接到同一个父类接口上，多对一，体现面向对象的抽象的威力。以串口设备为例，不管下层是 STM32、GD32 还是别的平台的，只要都是串口设备，都对接到 RT-Thread  的串口设备类——如图所绘，多个硬件对象对接同一个父类对象接口。同理，从设备驱动框架层到IO设备管理接口层，又是多对一，又是再一次的屏蔽差异，再一次的抽象。——面向对象的思想贯穿其中。

tips:
新增 BSP 设备驱动到 I/O 设备模型框架上时，开发者只需开发驱动层即可，设备驱动框架层和 I/O 设备管理层 RT-Thread 已写好了，无需改动，除非发现BUG或增加新的类别。