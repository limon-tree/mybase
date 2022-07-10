
## 消息队列

消息队列是另一种常用的线程间通讯方式，它能够接收来自线程或中断服务例程中不固定长度的消息，并把消息缓存在自己的内存空间中。其他线程也能够从消息队列中读取相应的消息，而当消息队列是空的时候，可以挂起读取线程。当有新的消息到达时，挂起的线程将被唤醒以接收并处理消息。

消息队列主要操作包括：通过函数 mq_open() 创建或者打开，调用 mq_send() 发送一条消息到消息队列，调用 mq_receive() 从消息队列获取一条消息，当消息队列不在使用时，可以调用 mq_unlink() 删除消息队列。

POSIX 消息队列主要用于进程间通信，RT-Thread 操作系统的 POSIX 消息队列主要是基于 RT-Thread 内核消息队列的一个封装，主要还是用于系统内线程间的通讯。使用方式和 RT-Thread 内核的消息队列差不多。

### 消息队列控制块

每个消息队列对应一个消息队列控制块，创建消息队列前需要先定义一个消息队列控制块。消息队列控制块定义在 mqueue.h 头文件里。

```c
struct mqdes
{
    rt_uint16_t refcount;  /* 引用计数 */
    rt_uint16_t unlinked;  /* 消息队列的分离状态，值为 1 表示消息队列已经分离 */
    rt_mq_t mq;        /* RT-Thread 消息队列控制块 */
    struct mqdes* next;    /* 指向下一个消息队列控制块 */
};
typedef struct mqdes* mqd_t;  /* 消息队列控制块指针类型重定义 */
```

### 创建或打开消息队列

```c
mqd_t mq_open(const char *name, int oflag, ...);
```

|  **参数** |       **描述**       |
|----------|----------------|
|  name |   消息队列名称   |
| oflag | 消息队列打开方式 |
|**返回**| ——          |
| 消息队列句柄        | 成功 |
| NULL      | 失败    |

此函数会根据消息队列的名字 name 创建一个新的消息队列或者打开一个已经存在的消息队列。Oflag 的可选值有 0、O_CREAT 或 O_CREAT\|O_EXCL。如果 Oflag 设置为 O_CREAT 则会创建一个新的消息队列。如果 Oflag 设置 O_CREAT\|O_EXCL，如果消息队列已经存在则会返回 NULL，如果不存在则会创建一个新的消息队列。如果 Oflag 设置为 0，消息队列不存在则会返回 NULL。

### 分离消息队列

```c
int mq_unlink(const char *name);
```

| **参数** |     **描述**     |
|----|------------|
| name | 消息队列名称 |
|**返回**| ——          |
| 0         | 成功 |
| -1        | 失败    |

此函数会根据消息队列名称 name 查找消息队列，若找到，则将消息队列置为分离状态，之后若持有计数为 0，则删除消息队列，并释放消息队列占有的资源。

### 关闭消息队列

```c
int mq_close(mqd_t mqdes);
```

|  **参数** |     **描述**     |
|----------|------------|
| mqdes | 消息队列句柄 |
|**返回**| ——          |
| 0         | 成功 |
| -1        | 失败    |

当一个线程终止时，会对其占用的消息队列执行此关闭操作。不论线程是自愿终止还是非自愿终止都会执行这个关闭操作，相当于是消息队列的持有计数减 1，若减 1 后持有计数为 0，且消息队列处于分离状态，则会删除 mqdes 消息队列并释放其占有的资源。

### 阻塞方式发送消息

```c
int mq_send(mqd_t mqdes,
            const char *msg_ptr,
            size_t msg_len,
            unsigned msg_prio);
```

|    **参数**   |                **描述**                |
|---------|----------------------------------|
|   mqdes   |       消息队列句柄, 不能为 NULL      |
|  sg_ptr  | 指向要发送的消息的指针，不能为 NULL |
|  msg_len |          发送的消息的长度          |
| msg_prio |         RT-Thread 未实现参数        |
|**返回**| ——          |
| 0         | 成功 |
| -1        | 失败    |

此函数用来向 mqdes 消息队列发送一条消息，是 rt_mq_send() 函数的封装。此函数把 msg_ptr 指向的消息添加到 mqdes 消息队列中，发送的消息长度 msg_len 必须小于或者等于创建消息队列时设置的最大消息长度。

如果消息队列已经满，即消息队列中的消息数量等于最大消息数，发送消息的的线程或者中断程序会收到一个错误码（-RT_EFULL）。

### 指定阻塞时间发送消息

```c
int mq_timedsend(mqd_t mqdes,
                const char *msg_ptr,
                size_t msg_len,
                unsigned msg_prio,
                const struct timespec *abs_timeout);
```

|     **参数**     |                        **描述**                       |
|------------|-------------------------------------------------|
|     mqdes    |              消息队列句柄, 不能为 NULL              |
|   msg_ptr   |         指向要发送的消息的指针，不能为 NULL        |
|   msg_len   |                  发送的消息的长度                 |
|   msg_prio  |                RT-Thread 未实现参数                |
| abs_timeout | 指定的等待时间，单位是操作系统时钟节拍（OS Tick） |
|**返回**| ——          |
| 0         | 成功 |
| -1        | 失败    |

目前 RT-Thread 不支持指定阻塞时间发送消息，但是函数接口已经实现，相当于调用 mq_send()。

### 阻塞方式接受消息

```c
ssize_t mq_receive(mqd_t mqdes,
                  char *msg_ptr,
                  size_t msg_len,
                  unsigned *msg_prio);
```

|    **参数**   |                **描述**                |
|---------|----------------------------------|
|   mqdes   |       消息队列句柄, 不能为 NULL      |
|  msg_ptr | 指向要发送的消息的指针，不能为 NULL |
|  msg_len |          发送的消息的长度          |
| msg_prio |         RT-Thread 未实现参数        |
|**返回**| ——          |
| 消息长度      | 成功 |
| -1        | 失败    |

此函数会把 mqdes 消息队列里面最老的消息移除消息队列，并把消息放到 msg_ptr 指向的内存里。如果消息队列为空，调用 mq_receive() 函数的线程将会阻塞，直到消息队列中消息可用。

### 指定阻塞时间接受消息

```c
ssize_t mq_timedreceive(mqd_t mqdes,
                        char *msg_ptr,
                        size_t msg_len,
                        unsigned *msg_prio,
                        const struct timespec *abs_timeout);
```

|     **参数**     |                        **描述**                       |
|------------|-------------------------------------------------|
|     mqdes    |              消息队列句柄, 不能为 NULL              |
|   msg_ptr   |         指向要发送的消息的指针，不能为 NULL        |
|   msg_len   |                  发送的消息的长度                 |
|   msg_prio  |                RT-Thread 未实现参数                |
| abs_timeout | 指定的等待时间，单位是操作系统时钟节拍（OS Tick） |
|**返回**| ——          |
| 消息长度      | 成功 |
| -1        | 失败    |

此函数和 mq_receive() 函数的区别在于，若消息队列为空，线程将阻塞 abs_timeout 时长，超时后函数直接返回 - 1，线程将被唤醒由阻塞态进入就绪态。

### 消息队列示例代码

这个程序会创建 3 个线程，线程 1 从消息队列接受消息，线程 2 和线程 3 往消息队列发送消息。

```c
#include <mqueue.h>
#include <stdio.h>

/* 线程控制块 */
static pthread_t tid1;
static pthread_t tid2;
static pthread_t tid3;
/* 消息队列句柄 */
static mqd_t mqueue;

/* 函数返回值检查函数 */
static void check_result(char* str,int result)
{
    if (0 == result)
    {
            printf("%s successfully!\n",str);
    }
    else
    {
            printf("%s failed! error code is %d\n",str,result);
    }
}
/* 线程 1 入口函数 */
static void* thread1_entry(void* parameter)
{
    char buf[128];
    int result;

    while (1)
    {
        /* 从消息队列中接收消息 */
        result = mq_receive(mqueue, &buf[0], sizeof(buf), 0);
        if (result != -1)
        {
            /* 输出内容 */
            printf("thread1 recv [%s]\n", buf);
        }

        /* 休眠 1 秒 */
       // sleep(1);
    }
}
/* 线程 2 入口函数 */
static void* thread2_entry(void* parameter)
{
    int i, result;
    char buf[] = "message2 No.x";

    while (1)
    {
       for (i = 0; i < 10; i++)
        {
            buf[sizeof(buf) - 2] = '0' + i;

            printf("thread2 send [%s]\n", buf);
            /* 发送消息到消息队列中 */
            result = mq_send(mqueue, &buf[0], sizeof(buf), 0);
            if (result == -1)
            {
                /* 消息队列满， 延迟 1s 时间 */
                printf("thread2:message queue is full, delay 1s\n");
                sleep(1);
            }
        }

        /* 休眠 2 秒 */
        sleep(2);
    }
}
/* 线程 3 入口函数 */
static void* thread3_entry(void* parameter)
{
    int i, result;
    char buf[] = "message3 No.x";

    while (1)
    {
       for (i = 0; i < 10; i++)
        {
            buf[sizeof(buf) - 2] = '0' + i;

            printf("thread3 send [%s]\n", buf);
            /* 发送消息到消息队列中 */
            result = mq_send(mqueue, &buf[0], sizeof(buf), 0);
            if (result == -1)
            {
                /* 消息队列满， 延迟 1s 时间 */
                printf("thread3:message queue is full, delay 1s\n");
                sleep(1);
            }
        }

        /* 休眠 2 秒 */
        sleep(2);
    }
}
/* 用户应用入口 */
int rt_application_init()
{
    int result;
    struct mq_attr mqstat;
    int oflag = O_CREAT|O_RDWR;
#define MSG_SIZE    128
#define MAX_MSG        128
    memset(&mqstat, 0, sizeof(mqstat));
    mqstat.mq_maxmsg = MAX_MSG;
    mqstat.mq_msgsize = MSG_SIZE;
    mqstat.mq_flags = 0;
    mqueue = mq_open("mqueue1",O_CREAT,0777,&mqstat);

    /* 创建线程 1, 线程入口是 thread1_entry, 属性参数设为 NULL 选择默认值，入口参数为 NULL*/
    result = pthread_create(&tid1,NULL,thread1_entry,NULL);
    check_result("thread1 created",result);

    /* 创建线程 2, 线程入口是 thread2_entry, 属性参数设为 NULL 选择默认值，入口参数为 NULL*/
    result = pthread_create(&tid2,NULL,thread2_entry,NULL);
    check_result("thread2 created",result);

    /* 创建线程 3, 线程入口是 thread3_entry, 属性参数设为 NULL 选择默认值，入口参数为 NULL*/
    result = pthread_create(&tid3,NULL,thread3_entry,NULL);
    check_result("thread3 created",result);


    return 0;
}
```
