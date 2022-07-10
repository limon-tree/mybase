# 传感器驱动开发指南

## 概述

### 目的与概述

本文档为 RT-Thread Sensor 驱动框架下传感器驱动的开发指南文档，给开发团队提供开发标准和规范。

### 阅读对象

- 进行传感器驱动开发的工程人员

> [!NOTE]
> 注：在阅读本篇文档之前，请先查看 [传感器驱动框架介绍](sensor_driver.md)。

## 开发指南

开发一个传感器驱动一般需要下面的几个步骤：调研与准备、开发、测试、提交。

开发过程可以参考已经支持的传感器，点击查看[支持的传感器列表](../../programming-manual/device/sensor/sensor_list.md)。

### 调研与准备

根据 datasheet 或其他途径，了解传感器的特性，并记录下来，如下面几种：

- 传感器类型
- 通讯接口（i2c/spi/...）
- 测量范围
- 最短测量周期
- 支持几种工作模式、电源模式

### 开发

开发的主要任务就是对接 Sensor 驱动框架的 ops 接口，然后注册为 Sensor 设备，进而能够通过驱动框架控制传感器的相关行为。

#### ops 接口对接

sensor 框架共给出了两个接口（`fetch_data` / `control`），需要在驱动中实现这两个接口。

**fetchdata**

作用： 获取传感器的数据。接口原型：

```c
rt_size_t (*fetch_data)(struct rt_sensor_device *sensor, void *buf, rt_size_t len);
```

Sensor 驱动框架当前默认支持 轮询(RT_DEVICE_FLAG_RDONLY)、中断(RT_DEVICE_FLAG_INT_RX)、FIFO(RT_DEVICE_FLAG_FIFO_RX) 这三种打开方式。需要在这里判断传感器的工作模式，然后再根据不同的模式返回传感器数据。

如下所示：

```c
static rt_size_t xxx_acc_fetch_data(struct rt_sensor_device *sensor, void *buf, rt_size_t len)
{
    if (sensor->parent.open_flag & RT_DEVICE_FLAG_RDONLY)
    {
        return _xxx_acc_polling_get_data(sensor, buf, len);
    }
    else if (sensor->parent.open_flag & RT_DEVICE_FLAG_INT_RX)
    {
        return _xxx_acc_int_get_data(sensor, buf, len);
    }
    else if (sensor->parent.open_flag & RT_DEVICE_FLAG_FIFO_RX)
    {
        return _xxx_acc_fifo_get_data(sensor, buf, len);
    }
    else
        return 0;
}
```

开发人员在返回数据时应先标识存储数据的数据类型，然后再填充数据域与时间戳，如下所示：

```c
sensor_data->type = RT_SENSOR_CLASS_ACCE
sensor_data->data.acce.x = acceleration.x;
sensor_data->data.acce.y = acceleration.y;
sensor_data->data.acce.z = acceleration.z;
sensor_data->timestamp = rt_sensor_get_ts();
```

> [!NOTE]
> 注：- 时间戳的获取函数请使用 Sensor 驱动框架提供的时间戳获取函数 `rt_sensor_get_ts`。
    - 在 FIFO 模式下底层数据可能会有耦合，需要使用 module，同时更新两个传感器的数据。
    - 要将数据的单位转换为 Sensor 驱动框架里规定的数据单位。

数据单位参考如下：

| 传感器名称 | 类型                      | 单位        | 说明                                   |
| ---------- | ------------------------- | ----------- | -------------------------------------- |
| 加速度计   | RT_SENSOR_CLASS_ACCE      | mg          | 1 g = 1000 mg, 1 g = 9.8 m/s2          |
| 陀螺仪     | RT_SENSOR_CLASS_GYRO      | mdps        | 1 dps = 1000 mdps, dps(度每秒)         |
| 磁力计     | RT_SENSOR_CLASS_MAG       | mGauss      | 1 G = 1000 mGauss(毫高斯)              |
| 环境光     | RT_SENSOR_CLASS_LIGHT     | lux         | 亮度流明值                             |
| 接近光     | RT_SENSOR_CLASS_PROXIMITY | centimeters | 代表物体到传感器的距离大小，单位：厘米 |
| 气压计     | RT_SENSOR_CLASS_BARO      | Pa          | 100 Pa = 1 hPa（百帕）                 |
| 温度计     | RT_SENSOR_CLASS_TEMP      | °c/10       | 0.1摄氏度                              |
| 湿度计     | RT_SENSOR_CLASS_HUMI      | ‰           | 相对湿度RH，一般用‰表示                |
| 心率计     | RT_SENSOR_CLASS_HR        | bpm         | 次数每分钟                             |
| 噪声       | RT_SENSOR_CLASS_NOISE     | HZ          | 频率单位                               |
| 计步计     | RT_SENSOR_CLASS_STEP      | 1           | 步数：无量纲单位 1                     |
| 力传感器   | RT_SENSOR_UNIT_MN         | mN          | 压力的大小：10^-3 N                    |

*注：后期会迭代增加新的传感器数据单位。*

**control**

```c
rt_err_t (*control)(struct rt_sensor_device *sensor, int cmd, void *arg);
```

传感器的控制就是依靠这个接口函数实现的，通过判断传入的命令字的不同执行不同的操作，目前支持以下命令字：

```c
#define  RT_SENSOR_CTRL_GET_ID            (0)  /* 读设备ID */
#define  RT_SENSOR_CTRL_GET_INFO          (1)  /* 获取设备信息（由框架实现，在驱动中不需要实现）*/
#define  RT_SENSOR_CTRL_SET_RANGE         (2)  /* 设置传感器测量范围 */
#define  RT_SENSOR_CTRL_SET_ODR           (3)  /* 设置传感器数据输出速率，unit is HZ */
#define  RT_SENSOR_CTRL_SET_MODE          (4)  /* 设置工作模式 */
#define  RT_SENSOR_CTRL_SET_POWER         (5)  /* 设置电源模式 */
#define  RT_SENSOR_CTRL_SELF_TEST         (6)  /* 自检 */
```

需要在驱动里实现这个函数，如下所示：

```c
static rt_err_t xxx_acc_control(struct rt_sensor_device *sensor, int cmd, void *args)
{
    rt_err_t result = RT_EOK;

    switch (cmd)
    {
    case RT_SENSOR_CTRL_GET_ID:
        result = _xxx_acc_get_id(sensor, args);
        break;
    case RT_SENSOR_CTRL_SET_RANGE:
        result = _xxx_acc_set_range(sensor, (rt_int32_t)args);
        break;
    case RT_SENSOR_CTRL_SET_ODR:
        result = _xxx_acc_set_odr(sensor, (rt_uint32_t)args & 0xffff);
        break;
    case RT_SENSOR_CTRL_SET_MODE:
        result = _xxx_acc_set_mode(sensor, (rt_uint32_t)args & 0xff);
        break;
    case RT_SENSOR_CTRL_SET_POWER:
        result = _xxx_acc_set_power(sensor, (rt_uint32_t)args & 0xff);
        break;
    case RT_SENSOR_CTRL_SELF_TEST:
        break;
    default:
        return -RT_ERROR;
    }
    return result;
}
```
> [!NOTE]
> 注：这里需要注意传来参数的数据类型是由 struct rt_sensor_config 这个结构体规定的，因此 `RT_SENSOR_CTRL_SET_RANGE` 这个命令传来的参数是 `rt_int32_t` 类型的，需要经过强转一次，才可以得到正确的参数。

然后 实现一个设备接口的结构体 ops 存储上面的接口函数，

```c
static struct rt_sensor_ops xxx_ops =
{
    xxx_acc_fetch_data,
    xxx_acc_control
};
```

#### 设备注册

完成 Sensor 的 ops 的对接之后还要注册一个 sensor 设备，这样上层才能找到这个传感器设备，进而进行控制。

设备的注册一共需要下面几步：创建一个 `rt_sensor_t` 的结构体指针，然后为结构体分配内存，并完成相关初始化。

```c
int rt_hw_xxx_init(const char *name, struct rt_sensor_config *cfg)
{
    rt_int8_t result;
    rt_sensor_t sensor = RT_NULL;

    sensor = rt_calloc(1, sizeof(struct rt_sensor_device));
    if (sensor == RT_NULL)
        return -1;

    sensor->info.type       = RT_SENSOR_CLASS_ACCE;
    sensor->info.vendor     = RT_SENSOR_VENDOR_STM;
    sensor->info.model      = "xxx_acce";
    sensor->info.unit       = RT_SENSOR_UNIT_MG;
    sensor->info.intf_type  = RT_SENSOR_INTF_I2C;
    sensor->info.range_max  = SENSOR_ACC_RANGE_16G;
    sensor->info.range_min  = SENSOR_ACC_RANGE_2G;
    sensor->info.period_min = 100;

    rt_memcpy(&sensor->config, cfg, sizeof(struct rt_sensor_config));
    sensor->ops = &sensor_ops;

    result = rt_hw_sensor_register(sensor, name, RT_DEVICE_FLAG_RDONLY | RT_DEVICE_FLAG_FIFO_RX, RT_NULL);
    if (result != RT_EOK)
    {
        LOG_E("device register err code: %d", result);
        rt_free(sensor);
        return -RT_ERROR;
    }

    LOG_I("acc sensor init success");
    return 0;
}
```

> [!NOTE]
> 注：- `rt_hw_sensor_register` 会为传入的 name 自动添加前缀，如`加速度计`类型的传感器会自动添加 `acce_` 的前缀。由于系统默认的设备名最长为 7 个字符，因此如果传入的名称超过3个字符的话会被裁掉。
    - 注册时如传感器支持 FIFO 的话，需要添加 RT_DEVICE_FLAG_FIFO_RX 的标志位。
    - 如果两个设备有耦合的话，需要利用 module 解耦。初始化一个 module，将两个传感器的设备控制块赋值到里面，并将 module 的地址赋值给两个传感器设备。框架会自动完成 module 锁的创建。

其中传入参数 `struct rt_sensor_config *cfg` 是用来解耦硬件的通讯接口的，通过在底层驱动初始化的时候传入这个参数，实现硬件接口的配置。里面包含 `struct rt_sensor_intf` 这个结构体，

```c
struct rt_sensor_intf
{
    char         *dev_name;   /* The name of the communication device */
    rt_uint8_t    type;       /* Communication interface type */
    void         *user_data;  /* Private data for the sensor. ex. i2c addr,spi cs,control I/O */
};
```

其中，type 表示接口的类型，dev_name 表示设备的名字，例如"i2c1"。user_data 是此接口类型的一些私有数据，如果是 I2C 的话，这里就是传感器对应的 i2c 地址，传入方式为 `(void*)0x55`。

在底层驱动初始化时，需要先初始化此结构体，然后作为参数传入，以便完成通讯接口的解耦。类似这样:

```c
#define irq_pin GET_PIN(B, 0)

int lps22hb_port(void)
{
    struct rt_sensor_config cfg;

    cfg.intf.dev_name = "i2c1";
    cfg.intf.user_data = (void *)0x55;
    cfg.irq_pin.pin = irq_pin;
    cfg.irq_pin.mode = PIN_MODE_INPUT_PULLDOWN;
    rt_hw_xxx_init("xxx", &cfg);

    return RT_EOK;
}
INIT_APP_EXPORT(lps22hb_port);
```
### 测试

1. 通过 list_device 命令查看对应设备是否注册成功。
2. 通过导出的测试函数 `sensor_polling/int/fifo <sensor_name> `，判断能否成功读取数据。
3. 测试其他的模式和控制接口

### 提交

Sensor 驱动需要按 `软件包` 的方式提交，具体的结构可以参考已有的 Sensor 软件包。具体的提交流程也可以参考文档中心：[软件包开发指南](https://www.rt-thread.org/document/site/development-guide/package/package/)

### 注意事项

- 动态分配内存时使用 `rt_calloc`，该 API 会将申请的内存初始化为 0，无需再手动清零。
- 静态定义的变量请赋初值，未使用的初始化为 0。
- 如果可能的话，请尽量支持多实例。需要注意下面的问题：
    - 不能出现全局变量
    - 所有的配置信息存储到 sensor 结构体里
    - sensor 结构体里没有的配置，利用 rt_device 的 user_data 来存储
