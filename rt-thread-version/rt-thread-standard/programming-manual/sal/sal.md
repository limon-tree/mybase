# SAL 套接字抽象层

## SAL 简介

为了适配更多的网络协议栈类型，避免系统对单一网络协议栈的依赖，RT-Thread 系统提供了一套 SAL（套接字抽象层）组件，该组件完成对不同网络协议栈或网络实现接口的抽象并对上层提供一组标准的 BSD Socket API，这样开发者只需要关心和使用网络应用层提供的网络接口，而无需关心底层具体网络协议栈类型和实现，极大的提高了系统的兼容性，方便开发者完成协议栈的适配和网络相关的开发。SAL 组件主要功能特点：

- 抽象、统一多种网络协议栈接口；
- 提供 Socket 层面的 TLS 加密传输特性;
- 支持标准 BSD Socket API；
- 统一的 FD 管理，便于使用 read/write  poll/select 来操作网络功能；

### SAL 网络框架

RT-Thread 的 网络框架结构如下所示：

![网络框架图](figures/network_frame.jpg)

最顶层是网络应用层，提供一套标准 BSD Socket API ，如 socket、connect 等函数，用于系统中大部分网络开发应用。

往下第二部分为 SAL 套接字抽象层，通过它 RT-Thread 系统能够适配下层不同的网络协议栈，并提供给上层统一的网络编程接口，方便不同协议栈的接入。套接字抽象层为上层应用层提供接口有：accept、connect、send、recv 等。

第三部分为 netdev 网卡层，主要作用是解决多网卡情况设备网络连接和网络管理相关问题，通过 netdev 网卡层用户可以统一管理各个网卡信息和网络连接状态，并且可以使用统一的网卡调试命令接口。

第四部分为协议栈层，该层包括几种常用的 TCP/IP 协议栈，例如嵌入式开发中常用的轻型 TCP/IP 协议栈 lwIP 以及 RT-Thread 自主研发的 AT Socket 网络功能实现等。这些协议栈或网络功能实现直接和硬件接触，完成数据从网络层到传输层的转化。

RT-Thread 的网络应用层提供的接口主要以标准 BSD Socket API 为主，这样能确保程序可以在 PC 上编写、调试，然后再移植到 RT-Thread 操作系统上。

### 工作原理

SAL 组件工作原理的介绍主要分为如下三部分：

- 多协议栈接入与接口函数统一抽象功能；
- SAL TLS 加密传输功能；

#### 多协议栈接入与接口函数统一抽象功能

对于不同的协议栈或网络功能实现，网络接口的名称可能各不相同，以 connect 连接函数为例，lwIP 协议栈中接口名称为 lwip_connect ，而 AT Socket 网络实现中接口名称为 at_connect。SAL 组件提供对不同协议栈或网络实现接口的抽象和统一，组件在 socket 创建时通过**判断传入的协议簇（domain）类型来判断使用的协议栈或网络功能**，完成 RT-Thread 系统中多协议的接入与使用。

目前 SAL 组件支持的协议栈或网络实现类型有：**lwIP 协议栈**、**AT Socket 协议栈**、**WIZnet 硬件 TCP/IP 协议栈**。

```c
int socket(int domain, int type, int protocol);
```

上述为标准 BSD Socket API 中 socket 创建函数的定义，`domain` 表示协议域又称为协议簇（family），用于判断使用哪种协议栈或网络实现，AT Socket 协议栈使用的簇类型为 **AF_AT**，lwIP 协议栈使用协议簇类型有 **AF_INET**等，WIZnet 协议栈使用的协议簇类型为 **AF_WIZ**。

对于不同的软件包，socket 传入的协议簇类型可能是固定的，不会随着 SAL 组件接入方式的不同而改变。**为了动态适配不同协议栈或网络实现的接入**，SAL 组件中对于每个协议栈或者网络实现提供两种协议簇类型匹配方式：**主协议簇类型和次协议簇类型**。socket 创建时先判断传入协议簇类型是否存在已经支持的主协议类型，如果是则使用对应协议栈或网络实现，如果不是判断次协议簇类型是否支持。目前系统支持协议簇类型如下：

```
lwIP 协议栈： family = AF_INET、sec_family = AF_INET
AT Socket 协议栈： family = AF_AT、sec_family = AF_INET
WIZnet 硬件 TCP/IP 协议栈： family = AF_WIZ、sec_family = AF_INET
```

SAL 组件主要作用是统一抽象底层 BSD Socket  API 接口，下面以 connect 函数调用流程为例说明 SAL 组件函数调用方式：

- connect：SAL 组件对外提供的抽象的 BSD Socket API，用于统一 fd 管理；
- sal_connect：SAL 组件中 connect 实现函数，用于调用底层协议栈注册的 operation 函数。
- lwip_connect：底层协议栈提供的层 connect 连接函数，在网卡初始化完成时注册到 SAL 组件中，最终调用的操作函数。

```c
/* SAL 组件为应用层提供的标准 BSD Socket API */
int connect(int s, const struct sockaddr *name, socklen_t namelen)
{
    /* 获取 SAL 套接字描述符 */
    int socket = dfs_net_getsocket(s);

    /* 通过 SAL 套接字描述符执行 sal_connect 函数 */
    return sal_connect(socket, name, namelen);
}

/* SAL 组件抽象函数接口实现 */
int sal_connect(int socket, const struct sockaddr *name, socklen_t namelen)
{
    struct sal_socket *sock;
    struct sal_proto_family *pf;
    int ret;

    /* 检查 SAL socket 结构体是否正常 */
    SAL_SOCKET_OBJ_GET(sock, socket);

    /* 检查当前 socket 网络连接状态是否正常  */
    SAL_NETDEV_IS_COMMONICABLE(sock->netdev);
    /* 检查当前 socket 对应的底层 operation 函数是否正常  */
    SAL_NETDEV_SOCKETOPS_VALID(sock->netdev, pf, connect);

    /* 执行底层注册的 connect operation 函数 */
    ret = pf->skt_ops->connect((int) sock->user_data, name, namelen);
#ifdef SAL_USING_TLS
    if (ret >= 0 && SAL_SOCKOPS_PROTO_TLS_VALID(sock, connect))
    {
        if (proto_tls->ops->connect(sock->user_data_tls) < 0)
        {
            return -1;
        }
        return ret;
    }
#endif
    return ret;
}

/* lwIP 协议栈函数底层 connect 函数实现 */
int lwip_connect(int socket, const struct sockaddr *name, socklen_t namelen)
{
    ...
}
```

#### SAL TLS 加密传输功能

**1. SAL TLS 功能介绍**

在 TCP、UDP等协议数据传输时，由于数据包是明文的，所以很可能被其他人拦截并解析出信息，这给信息的安全传输带来很大的影响。为了解决此类问题，一般需要用户在应用层和传输层之间添加 SSL/TLS 协议。

TLS（Transport Layer Security，传输层安全协议) 是建立在传输层 TCP 协议之上的协议，其前身是 SSL（Secure Socket Layer，安全套接字层 ），主要作用是将应用层的报文进行非对称加密后再由 TCP 协议进行传输，实现了数据的加密安全交互。

目前常用的 TLS 方式：**MbedTLS、OpenSSL、s2n** 等，但是对于不同的加密方式，需要使用其指定的加密接口和流程进行加密，对于部分应用层协议的移植较为复杂。因此 SAL TLS 功能产生，主要作用是**提供 Socket 层面的 TLS 加密传输特性，抽象多种 TLS 处理方式，提供统一的接口用于完成 TLS 数据交互**。

**2. SAL TLS 功能使用方式**

使用流程如下：

- 配置开启任意网络协议栈支持（如 lwIP 协议栈）；

- 配置开启 MbedTLS 软件包（目前只支持 MbedTLS 类型加密方式）；

- 配置开启 SAL_TLS 功能支持（如下配置选项章节所示）；

配置完成之后，只要在 socket 创建时传入的 `protocol` 类型使用 **PROTOCOL_TLS** 或 **PROTOCOL_DTLS **，即可使用标准 BSD Socket API 接口，完成 TLS 连接的建立和数据的收发。示例代码如下所示：

```c
#include <stdio.h>
#include <string.h>

#include <rtthread.h>
#include <sys/socket.h>
#include <netdb.h>

/* RT-Thread 官网，支持 TLS 功能 */
#define SAL_TLS_HOST    "www.rt-thread.org"
#define SAL_TLS_PORT    443
#define SAL_TLS_BUFSZ   1024

static const char *send_data = "GET /download/rt-thread.txt HTTP/1.1\r\n"
    "Host: www.rt-thread.org\r\n"
    "User-Agent: rtthread/4.0.1 rtt\r\n\r\n";

void sal_tls_test(void)
{
    int ret, i;
    char *recv_data;
    struct hostent *host;
    int sock = -1, bytes_received;
    struct sockaddr_in server_addr;

    /* 通过函数入口参数url获得host地址（如果是域名，会做域名解析） */
    host = gethostbyname(SAL_TLS_HOST);

    recv_data = rt_calloc(1, SAL_TLS_BUFSZ);
    if (recv_data == RT_NULL)
    {
        rt_kprintf("No memory\n");
        return;
    }

    /* 创建一个socket，类型是SOCKET_STREAM，TCP 协议, TLS 类型 */
    if ((sock = socket(AF_INET, SOCK_STREAM, PROTOCOL_TLS)) < 0)
    {
        rt_kprintf("Socket error\n");
        goto __exit;
    }

    /* 初始化预连接的服务端地址 */
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SAL_TLS_PORT);
    server_addr.sin_addr = *((struct in_addr *)host->h_addr);
    rt_memset(&(server_addr.sin_zero), 0, sizeof(server_addr.sin_zero));

    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) < 0)
    {
        rt_kprintf("Connect fail!\n");
        goto __exit;
    }

    /* 发送数据到 socket 连接 */
    ret = send(sock, send_data, strlen(send_data), 0);
    if (ret <= 0)
    {
        rt_kprintf("send error,close the socket.\n");
        goto __exit;
    }

    /* 接收并打印响应的数据，使用加密数据传输 */
    bytes_received = recv(sock, recv_data, SAL_TLS_BUFSZ  - 1, 0);
    if (bytes_received <= 0)
    {
        rt_kprintf("received error,close the socket.\n");
        goto __exit;
    }

    rt_kprintf("recv data:\n");
    for (i = 0; i < bytes_received; i++)
    {
        rt_kprintf("%c", recv_data[i]);
    }

__exit:
    if (recv_data)
        rt_free(recv_data);

    if (sock >= 0)
        closesocket(sock);
}

#ifdef FINSH_USING_MSH
#include <finsh.h>
MSH_CMD_EXPORT(sal_tls_test, SAL TLS function test);
#endif /* FINSH_USING_MSH */
```

### 配置选项

当我们使用 SAL 组件时需要在 rtconfig.h 中定义如下宏定义：

|   **宏定义**   | **描述**                                 |
|--------------------------------|--------------------------------|
| RT_USING_SAL  | 开启 SAL 功能                         |
| SAL_USING_LWIP  | 开启 lwIP 协议栈支持     |
| SAL_USING_AT | 开启 AT Socket 协议栈支持    |
| SAL_USING_TLS | 开启 SAL TLS 功能支持 |
| SAL_USING_POSIX   | 开启 POSIX 文件系统相关函数支持，如 read、write、select/poll 等 |

目前 SAL 抽象层支持 lwIP 协 议栈、 AT Socket 协议栈和 WIZnet 硬件 TCP/IP 协议栈，系统中开启 SAL 需要至少开启一种协议栈支持。

上面配置选项可以直接在 `rtconfig.h` 文件中添加使用，也可以通过组件包管理工具 ENV 配置选项加入，ENV 工具中具体配置路径如下：

```c
RT-Thread Components  --->
    Network  --->
        Socket abstraction layer  --->
        [*] Enable socket abstraction layer
            protocol stack implement --->
            [ ] Support lwIP stack
            [ ] Support AT Commands stack
            [ ] Support MbedTLS protocol
        [*]    Enable BSD socket operated by file system API
```

配置完成可以通过 scons 命令重新生成功能，完成 SAL 组件的添加。

## 初始化 ##

配置开启 SAL 选项之后，需要在启动时对它进行初始化，开启 SAL 功能，如果程序中已经使用了组件自动初始化，则不再需要额外进行单独的初始化，否则需要在初始化任务中调用如下函数：

```c
int sal_init(void);
```

该初始化函数主要是对 SAL 组件进行初始，支持组件重复初始化判断，完成对组件中使用的互斥锁等资源的初始化。 SAL 组件中没有创建新的线程，这也意味着 SAL 组件资源占用极小，目前**SAL 组件资源占用为 ROM 2.8K 和 RAM 0.6K**。


## BSD Socket API 介绍 ##

SAL 组件抽象出标准 BSD Socket API 接口，如下是对常用网络接口的介绍：

### 创建套接字（socket）

```c
int socket(int domain, int type, int protocol);
```

|   **参数**   | **描述**                                 |
|--------|-------------------------------------|
|  domain  | 协议族类型                           |
|   type   | 协议类型                             |
| protocol | 实际使用的运输层协议                 |
|   **返回**   | --                                 |
|  >=0  | 成功，返回一个代表套接字描述符的整数 |
|    -1    | 失败                                 |

该函数用于根据指定的地址族、数据类型和协议来分配一个套接字描述符及其所用的资源。

**domain / 协议族类型：**

-   AF_INET： IPv4
-   AF_INET6： IPv6

**type / 协议类型：**

-   SOCK_STREAM：流套接字
-   SOCK_DGRAM： 数据报套接字
-   SOCK_RAW： 原始套接字

### 绑定套接字（bind）

```c
int bind(int s, const struct sockaddr *name, socklen_t namelen);
```

|    **参数**   | **描述**                                         |
|---------|---------------------------------------------|
|     s     | 套接字描述符                                 |
|    name   | 指向 sockaddr 结构体的指针，代表要绑定的地址 |
| namelen | sockaddr 结构体的长度                        |
|    **返回**   | --                                         |
|     0     | 成功                                         |
|     -1    | 失败                                         |

该函数用于将端口号和 IP 地址绑定带指定套接字上。

SAL 组件依赖 netdev 组件，当使用 `bind()` 函数时，可以通过 netdev 网卡名称获取网卡对象中 IP 地址信息，用于将创建的 Socket 套接字绑定到指定的网卡对象。下面示例完成通过传入的网卡名称绑定该网卡 IP 地址并和服务器进行连接的过程：

```c
#include <rtthread.h>
#include <arpa/inet.h>
#include <netdev.h>

#define SERVER_HOST   "192.168.1.123"
#define SERVER_PORT   1234

static int bing_test(int argc, char **argv)
{
    struct sockaddr_in client_addr;
    struct sockaddr_in server_addr;
    struct netdev *netdev = RT_NULL;
    int sockfd = -1;

    if (argc != 2)
    {
        rt_kprintf("bind_test [netdev_name]  --bind network interface device by name.\n");
        return -RT_ERROR;
    }

    /* 通过名称获取 netdev 网卡对象 */
    netdev = netdev_get_by_name(argv[1]);
    if (netdev == RT_NULL)
    {
        rt_kprintf("get network interface device(%s) failed.\n", argv[1]);
        return -RT_ERROR;
    }

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        rt_kprintf("Socket create failed.\n");
        return -RT_ERROR;
    }

    /* 初始化需要绑定的客户端地址 */
    client_addr.sin_family = AF_INET;
    client_addr.sin_port = htons(8080);
    /* 获取网卡对象中 IP 地址信息 */
    client_addr.sin_addr.s_addr = netdev->ip_addr.addr;
    rt_memset(&(client_addr.sin_zero), 0, sizeof(client_addr.sin_zero));

    if (bind(sockfd, (struct sockaddr *)&client_addr, sizeof(struct sockaddr)) < 0)
    {
        rt_kprintf("socket bind failed.\n");
        closesocket(sockfd);
        return -RT_ERROR;
    }
    rt_kprintf("socket bind network interface device(%s) success!\n", netdev->name);

    /* 初始化预连接的服务端地址 */
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = inet_addr(SERVER_HOST);
    rt_memset(&(server_addr.sin_zero), 0, sizeof(server_addr.sin_zero));

    /* 连接到服务端 */
    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) < 0)
    {
        rt_kprintf("socket connect failed!\n");
        closesocket(sockfd);
        return -RT_ERROR;
    }
    else
    {
        rt_kprintf("socket connect success!\n");
    }

    /* 关闭连接 */
    closesocket(sockfd);
    return RT_EOK;
}

#ifdef FINSH_USING_MSH
#include <finsh.h>
MSH_CMD_EXPORT(bing_test, bind network interface device test);
#endif /* FINSH_USING_MSH */
```

### 监听套接字（listen）

```c
int listen(int s, int backlog);
```

|   **参数**  | **描述**                           |
|-------|-------------------------------|
|    s    | 套接字描述符                   |
| backlog | 表示一次能够等待的最大连接数目 |
|   **返回**  | --                           |
|    0    | 成功                           |
|    -1   | 失败                           |

该函数用于 TCP 服务器监听指定套接字连接。

### 接收连接（accept）

```c
int accept(int s, struct sockaddr *addr, socklen_t *addrlen);
```

|   **参数**  | **描述**                           |
|-------|-------------------------------|
|    s    | 套接字描述符                   |
|   addr  | 表示一次能够等待的最大连接数目 |
| addrlen | 客户端设备地址结构体的长度     |
|   **返回**  | --                           |
|    0    | 成功，返回新创建的套接字描述符 |
|    -1   | 失败                           |

当应用程序监听来自其他主机的连接时，使用 `accept()` 函数初始化连接，`accept()` 为每个连接创立新的套接字并从监听队列中移除这个连接。

### 建立连接（connect）

```c
int connect(int s, const struct sockaddr *name, socklen_t namelen);
```

|   **参数**  | **描述**            |
|-------|-------------------------------|
|    s    | 套接字描述符                   |
|   name  | 服务器地址信息                 |
| namelen | 服务器地址结构体的长度         |
|   **返回**  | --                    |
|    0    | 成功，返回新创建的套接字描述符 |
|    -1   | 失败                    |

该函数用于建立与指定 socket 的连接。

### TCP 数据发送（send）

```c
int send(int s, const void *dataptr, size_t size, int flags);
```

|   **参数**  | **描述**                       |
|-------|---------------------------|
|    s    | 套接字描述符               |
| dataptr | 发送的数据指针             |
|   size  | 发送的数据长度             |
|  flags  | 标志，一般为 0              |
|   **返回**  | --                       |
|  >0  | 成功，返回发送的数据的长度 |
|  <=0 | 失败                       |

该函数常用于 TCP 连接发送数据。

### TCP 数据接收（recv）

```c
int recv(int s, void *mem, size_t len, int flags);
```

|  **参数** | **描述**                       |
|-----|---------------------------|
|   s   | 套接字描述符               |
|  mem  | 接收的数据指针             |
|  len  | 接收的数据长度             |
| flags | 标志，一般为 0              |
|  **返回** | --                       |
| >0 | 成功，返回接收的数据的长度 |
|   =0  | 目标地址已传输完并关闭连接 |
| <0 | 失败                       |

该函数用于 TCP 连接接收数据。

### UDP 数据发送（sendto）

```c
int sendto(int s, const void *dataptr, size_t size, int flags, const struct sockaddr *to, socklen_t tolen);
```

|   **参数**  | **描述**                       |
|-------|---------------------------|
|    s    | 套接字描述符               |
| dataptr | 发送的数据指针             |
|   size  | 发送的数据长度             |
|  flags  | 标志，一般为 0              |
|    to   | 目标地址结构体指针         |
|  tolen  | 目标地址结构体长度         |
|   **返回**  | --                       |
|  >0  | 成功，返回发送的数据的长度 |
|  <=0 | 失败                       |

该函数用于 UDP 连接发送数据。

### UDP 数据接收（recvfrom）

```c
int recvfrom(int s, void *mem, size_t len, int flags, struct sockaddr *from, socklen_t *fromlen);
```

|   **参数**  | **描述**                       |
|-------|---------------------------|
|    s    | 套接字描述符               |
|   mem   | 接收的数据指针             |
|   len   | 接收的数据长度             |
|  flags  | 标志，一般为 0              |
|   from  | 接收地址结构体指针         |
| fromlen | 接收地址结构体长度         |
|   **返回**  | --                       |
|  >0  | 成功，返回接收的数据的长度 |
|    =0   | 接收地址已传输完并关闭连接 |
|  <0  | 失败                       |

该函数用于 UDP 连接接收数据。

### 关闭套接字（closesocket）

```c
int closesocket(int s);
```

| **参数** | **描述**         |
|----|-------------|
|   s  | 套接字描述符 |
| **返回** | --         |
|   0  | 成功         |
|  -1  | 失败         |

该函数用于关闭连接，释放资源。

### 按设置关闭套接字（shutdown）

```c
int shutdown(int s, int how);
```

| **参数** | **描述**             |
|----|-----------------|
|   s  | 套接字描述符     |
|  how | 套接字控制的方式 |
| **返回** | --             |
|   0  | 成功             |
|  -1  | 失败             |

该函数提供更多的权限控制套接字的关闭过程。

**how / 套接字控制的方式：**

-   0： 停止接收当前数据，并拒绝以后的数据接收；
-   1： 停止发送数据，并丢弃未发送的数据；
-   2： 停止接收和发送数据。

### 设置套接字选项（setsockopt）

```c
int setsockopt(int s, int level, int optname, const void *optval, socklen_t optlen);
```

|   **参数**  | **描述**                   |
|-------|-----------------------|
|    s    | 套接字描述符           |
|  level  | 协议栈配置选项         |
| optname | 需要设置的选项名       |
|  optval | 设置选项值的缓冲区地址 |
|  optlen | 设置选项值的缓冲区长度 |
|   **返回**  | --                   |
|    =0   | 成功                   |
|  <0  | 失败                   |

该函数用于设置套接字模式，修改套接字配置选项。

**level / 协议栈配置选项：**

-   SOL_SOCKET：套接字层
-   IPPROTO_TCP：TCP 层
-   IPPROTO_IP：IP 层

**optname / 需要设置的选项名 :**

-   SO_KEEPALIVE：设置保持连接选项
-   SO_RCVTIMEO：设置套接字数据接收超时
-   SO_SNDTIMEO：设置套接数据发送超时

### 获取套接字选项（getsockopt）

```c
int getsockopt(int s, int level, int optname, void *optval, socklen_t *optlen);
```

|   **参数**  | **描述**                       |
|-------|---------------------------|
|    s    | 套接字描述符               |
|  level  | 协议栈配置选项             |
| optname | 需要设置的选项名           |
|  optval | 获取选项值的缓冲区地址     |
|  optlen | 获取选项值的缓冲区长度地址 |
|   **返回**  | --                       |
|    =0   | 成功                       |
|  <0  | 失败                       |

该函数用于获取套接字配置选项。

### 获取远端地址信息（getpeername）

```c
int getpeername(int s, struct sockaddr *name, socklen_t *namelen);
```

|   **参数**  | **描述**                     |
|-------|-------------------------|
|    s    | 套接字描述符             |
|   name  | 接收信息的地址结构体指针 |
| namelen | 接收信息的地址结构体长度 |
|   **返回**  | --                     |
|    =0   | 成功                     |
|  <0  | 失败                     |

该函数用于获取与套接字相连的远端地址信息。

### 获取本地地址信息（getsockname）

```c
int getsockname(int s, struct sockaddr *name, socklen_t *namelen);
```

|   **参数**  | **描述**                     |
|-------|-------------------------|
|    s    | 套接字描述符             |
|   name  | 接收信息的地址结构体指针 |
| namelen | 接收信息的地址结构体长度 |
|   **返回**  | --                     |
|    =0   | 成功                     |
|  <0  | 失败                     |

该函数用于获取本地套接字地址信息。

### 配置套接字参数（ioctlsocket）

```c
int ioctlsocket(int s, long cmd, void *arg);
```

|  **参数** | **描述**             |
|-----|-----------------|
|   s   | 套接字描述符     |
|  cmd  | 套接字操作命令   |
|  arg  | 操作命令所带参数 |
|  **返回** | --             |
|   =0  | 成功             |
| <0 | 失败             |

该函数用于设置套接字控制模式。

**cmd 支持下列命令**

-   FIONBIO：开启或关闭套接字的非阻塞模式，arg 参数 1 为开启非阻塞，0
    为关闭非阻塞。

## 网络协议栈接入方式

网络协议栈或网络功能实现的接入，主要是对协议簇结构体的初始化和注册处理, 并且添加到 SAL 组件中协议簇列表中，协议簇结构体定义如下：

```c
/* network interface socket opreations */
struct sal_socket_ops
{
    int (*socket)     (int domain, int type, int protocol);
    int (*closesocket)(int s);
    int (*bind)       (int s, const struct sockaddr *name, socklen_t namelen);
    int (*listen)     (int s, int backlog);
    int (*connect)    (int s, const struct sockaddr *name, socklen_t namelen);
    int (*accept)     (int s, struct sockaddr *addr, socklen_t *addrlen);
    int (*sendto)     (int s, const void *data, size_t size, int flags, const struct sockaddr *to, socklen_t tolen);
    int (*recvfrom)   (int s, void *mem, size_t len, int flags, struct sockaddr *from, socklen_t *fromlen);
    int (*getsockopt) (int s, int level, int optname, void *optval, socklen_t *optlen);
    int (*setsockopt) (int s, int level, int optname, const void *optval, socklen_t optlen);
    int (*shutdown)   (int s, int how);
    int (*getpeername)(int s, struct sockaddr *name, socklen_t *namelen);
    int (*getsockname)(int s, struct sockaddr *name, socklen_t *namelen);
    int (*ioctlsocket)(int s, long cmd, void *arg);
#ifdef SAL_USING_POSIX
    int (*poll)       (struct dfs_fd *file, struct rt_pollreq *req);
#endif
};

/* sal network database name resolving */
struct sal_netdb_ops
{
    struct hostent* (*gethostbyname)  (const char *name);
    int             (*gethostbyname_r)(const char *name, struct hostent *ret, char *buf, size_t buflen, struct hostent **result, int *h_errnop);
    int             (*getaddrinfo)    (const char *nodename, const char *servname, const struct addrinfo *hints, struct addrinfo **res);
    void            (*freeaddrinfo)   (struct addrinfo *ai);
};

/* 协议簇结构体定义 */
struct sal_proto_family
{
    int family;                                  /* primary protocol families type */
    int sec_family;                              /* secondary protocol families type */
    const struct sal_socket_ops *skt_ops;        /* socket opreations */
    const struct sal_netdb_ops *netdb_ops;       /* network database opreations */
};

```

- `family`： 每个协议栈支持的主协议簇类型，例如 lwIP 的为 AF_INET ，AT Socket 为 AF_AT， WIZnet 为 AF_WIZ。
- `sec_family`：每个协议栈支持的次协议簇类型，用于支持单个协议栈或网络实现时，匹配软件包中其他类型的协议簇类型。
- `skt_ops`： 定义 socket 相关执行函数，如connect、send、recv 等，每种协议簇都有一组不同的实现方式。
- `netdb_ops`：定义非 socket 相关执行函数，如 gethostbyname、getaddrinfo、 freeaddrinfo等，每种协议簇都有一组不同的实现方式。

以下为 AT Socket 网络实现的接入注册流程，开发者可参考实现其他的协议栈或网络实现的接入：

```c
#include <rtthread.h>
#include <netdb.h>
#include <sal.h>            /* SAL 组件结构体存放头文件 */
#include <at_socket.h>      /* AT Socket 相关头文件 */
#include <af_inet.h>

#include <netdev.h>         /* 网卡功能相关头文件 */

#ifdef SAL_USING_POSIX
#include <dfs_poll.h>       /* poll 函数实现相关头文件 */
#endif

#ifdef SAL_USING_AT

/* 自定义的 poll 执行函数，用于 poll 中处理接收的事件 */
static int at_poll(struct dfs_fd *file, struct rt_pollreq *req)
{
    int mask = 0;
    struct at_socket *sock;
    struct socket *sal_sock;

    sal_sock = sal_get_socket((int) file->data);
    if(!sal_sock)
    {
        return -1;
    }

    sock = at_get_socket((int)sal_sock->user_data);
    if (sock != NULL)
    {
        rt_base_t level;

        rt_poll_add(&sock->wait_head, req);

        level = rt_hw_interrupt_disable();
        if (sock->rcvevent)
        {
            mask |= POLLIN;
        }
        if (sock->sendevent)
        {
            mask |= POLLOUT;
        }
        if (sock->errevent)
        {
            mask |= POLLERR;
        }
        rt_hw_interrupt_enable(level);
    }

    return mask;
}
#endif

/* 定义和赋值 Socket 执行函数，SAL 组件执行相关函数时调用该注册的底层函数 */
static const struct proto_ops at_inet_stream_ops =
{
    at_socket,
    at_closesocket,
    at_bind,
    NULL,
    at_connect,
    NULL,
    at_sendto,
    at_recvfrom,
    at_getsockopt,
    at_setsockopt,
    at_shutdown,
    NULL,
    NULL,
    NULL,

#ifdef SAL_USING_POSIX
    at_poll,
#else
    NULL,
#endif /* SAL_USING_POSIX */
};

static const struct sal_netdb_ops at_netdb_ops =
{
    at_gethostbyname,
    NULL,
    at_getaddrinfo,
    at_freeaddrinfo,
};

/* 定义和赋值 AT Socket 协议簇结构体 */
static const struct sal_proto_family at_inet_family =
{
    AF_AT,
    AF_INET,
    &at_socket_ops,
    &at_netdb_ops,
};

/* 用于设置网卡设备中协议簇相关信息 */
int sal_at_netdev_set_pf_info(struct netdev *netdev)
{
    RT_ASSERT(netdev);

    netdev->sal_user_data = (void *) &at_inet_family;
    return 0;
}

#endif /* SAL_USING_AT */
```
