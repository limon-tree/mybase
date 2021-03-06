## Pthreads 简介

POSIX Threads 简称 Pthreads，POSIX 是 “Portable Operating System
Interface”（可移植操作系统接口） 的缩写，POSIX 是 IEEE Computer
Society 为了提高不同操作系统的兼容性和应用程序的可移植性而制定的一套标准。Pthreads 是线程的 POSIX 标准，被定义在 POSIX.1c,
Threads extensions (IEEE
Std1003.1c-1995)标准里，该标准定义了一套 C 程序语言的类型、函数和常量。定义在 pthread.h 头文件和一个线程库里，大约有 100 个 API，所有 API 都带有 “pthread_” 前缀，可以分为 4 大类：

-   线程管理（Thread
    management）：包括线程创建（creating）、分离（detaching）、连接（joining）及设置和查询线程属性的函数等。

-   互斥锁（Mutex）：“mutual
    exclusion” 的缩写，用了限制线程对共享数据的访问，保护共享数据的完整性。包括创建、销毁、锁定和解锁互斥锁及一些用于设置或修改互斥量属性等函数。

-   条件变量（Condition
    variable）：用于共享一个互斥量的线程间的通信。包括条件变量的创建、销毁、等待和发送信号（signal）等函数。

-   读写锁（read/write
    lock）和屏障（barrier）：包括读写锁和屏障的创建、销毁、等待及相关属性设置等函数。

-   POSIX 信号量（semaphore）和 Pthreads 一起使用，但不是 Pthreads 标准定义的一部分，被定义在 POSIX.1b,
    Real-time extensions (IEEE
    Std1003.1b-1993)标准里。因此信号量相关函数的前缀是 “sem_” 而不是“pthread_”。

-   消息队列（Message
    queue）和信号量一样，和 Pthreads 一起使用，也不是 Pthreads 标准定义的一部分，被定义在 IEEE
    Std 1003.1-2001 标准里。消息队列相关函数的前缀是 “mq_”。

|        函数前缀        |         函数组         |
|----------------------|----------------------|
|        `pthread_ `      | 线程本身和各种相关函数 |
|     `pthread_attr_`    |      线程属性对象      |
|    `Pthread_mutex_`    |         互斥锁         |
|  `pthread_mutexattr_` |     互斥锁属性对象     |
|     `pthread_cond_ `   |        条件变量        |
|   `pthread_condattr_`  |    条件变量属性对象    |
|    `pthread_rwlock_`   |         读写锁         |
|  `pthread_rwlockattr_` |     读写锁属性对象     |
|     `pthread_spin_`    |         自旋锁         |
|  `pthread_barrier_ `  |          屏障          |
| `pthread_barrierattr_` |      屏障属性对象      |
|          `sem_`         |         信号量         |
|          `mq_ `         |        消息队列        |

绝大部分 Pthreads 的函数执行成功则返回 0 值，不成功则返回一个包含在 `errno.h` 头文件中的错误代码。很多操作系统都支持 Pthreads，比如 Linux、MacOSX、 Android
和 Solaris，因此使用 Pthreads 的函数编写的应用程序有很好的可移植性，可以在很多支持 Pthreads 的平台上直接编译运行。

### 在 RT-Thread 中使用 POSIX

在 RT-Thread 中使用 POSIX API 接口包括几个部分：libc（例如 newlib），filesystem，pthread 等。

![image-20220419112531014](../figures/menuconfig_pthread.png)

1. 在 menuconfig 中打开 pthread ，设定支持得最大 pthread 数目
2. 在用户代码中，即可使用 ```pthread.h, sche.h ``` 这些 pthread 提供的头文件来编程

RT-Thread 实现了 Pthreads 的大部分函数和常量，按照 POSIX 标准定义在 pthread.h、mqueue.h、semaphore.h 和 sched.h 头文件里。Pthreads 是 libc 的一个子库，RT-Thread 中的 Pthreads 是基于 RT-Thread 内核函数的封装，使其符合 POSIX 标准。后续章节会详细介绍 RT-Thread 中实现的 Pthreads 函数及相关功能。


## 线程

### 线程句柄

```c
typedef rt_thread_t pthread_t;
```

pthread_t 是 rt_thread_t 类型的重定义，定义在 pthread.h 头文件里。rt_thread_t 是 RT-Thread 的线程句柄（或线程标识符），是指向线程控制块的指针。在创建线程前需要先定义一个 pthread_t 类型的变量。每个线程都对应了自己的线程控制块，线程控制块是操作系统用于控制线程的一个数据结构，它存放了线程的一些信息，例如优先级，线程名称和线程堆栈地址等。线程控制块及线程具体信息在 RT-Thread 编程手册的线程调度与管理一章有详细的介绍。

### 创建线程

```c
int pthread_create (pthread_t *tid,
                   const pthread_attr_t *attr,
                   void *(*start) (void *), void *arg);
```

|  **参数** |                          **描述**                          |
|----------|------------------------------------------------------|
|  tid  |       指向线程句柄 (线程标识符) 的指针，不能为 NULL       |
|  attr | 指向线程属性的指针，如果使用 NULL，则使用默认的线程属性 |
| start |                    线程入口函数地址                    |
|  arg  |                传递给线程入口函数的参数                |
|**返回**| ——                                 |
| 0         | 成功 |
| EINVAL  | 参数无效             |
| ENOMEM  | 动态分配内存失败            |

此函数创建一个 pthread 线程。此函数会动态分配 POSIX 线程数据块和 RT-Thread 线程控制块，并把线程控制块的起始地址（线程 ID）保存在参数 tid 指向的内存里，此线程标识符可用于在其他线程中操作此线程；并把 attr 指向的线程属性、start 指向的线程入口函数及入口函数参数 arg 保存在线程数据块和线程控制块里。如果线程创建成功，线程立刻进入就绪态，参与系统的调度，如果线程创建失败，则会释放之前线程占有的资源。

关于线程属性及相关函数会在线程高级编程一章里有详细介绍，一般情况下采用默认属性就可以。

> [!NOTE]
> 注：创建出 pthread 线程后，如果线程需要重复创建使用，需要设置 pthread 线程为 detach 模式，或者使用 pthread_join 等待创建后的 pthread 线程结束。

#### 创建线程示例代码

以下程序会初始化 2 个线程，它们拥有共同的入口函数，但是它们的入口参数不相同。其他的，它们具备相同的优先级，并以时间片进行轮转调度。

```c
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>

/* 线程控制块 */
static pthread_t tid1;
static pthread_t tid2;

/* 函数返回值检查 */
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

/* 线程入口函数 */
static void* thread_entry(void* parameter)
{
    int count = 0;
    int no = (int) parameter; /* 获得线程的入口参数 */

    while (1)
    {
        /* 打印输出线程计数值 */
        printf("thread%d count: %d\n", no, count ++);

        sleep(2);    /* 休眠 2 秒 */
    }
}

/* 用户应用入口 */
int rt_application_init()
{
    int result;

    /* 创建线程 1, 属性为默认值，入口函数是 thread_entry，入口函数参数是 1 */
    result = pthread_create(&tid1,NULL,thread_entry,(void*)1);
    check_result("thread1 created", result);

    /* 创建线程 2, 属性为默认值，入口函数是 thread_entry，入口函数参数是 2 */
    result = pthread_create(&tid2,NULL,thread_entry,(void*)2);
    check_result("thread2 created", result);

    return 0;
}
```

### 脱离线程

```c
int pthread_detach (pthread_t thread);
```

|  **参数**  |          **描述**          |
|------|----------------------|
| thread | 线程句柄（线程标识符） |
|**返回**| ——                                 |
| 0         | 成功 |

调用此函数，如果 pthread 线程没有结束，则将 thread 线程属性的分离状态设置为 detached；当 thread 线程已经结束时，系统将回收 pthread 线程占用的资源。

使用方法：子线程调用 pthread_detach(pthread_self())（pthread_self() 返回当前调用线程的线程句柄），或者其他线程调用 pthread_detach(thread_id)。关于线程属性的分离状态会在后面详细介绍。

> [!NOTE]
> 注：一旦线程属性的分离状态设置为 detached，该线程不能被 pthread_join() 函数等待或者重新被设置为 detached。

#### 脱离线程示例代码

以下程序会初始化 2 个线程，它们拥有相同的优先级，并按照时间片轮转调度。2 个线程都会被设置为脱离状态，2 个线程循环打印 3 次信息后自动退出，退出后系统将会自动回收其资源。

```c
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>

/* 线程控制块 */
static pthread_t tid1;
static pthread_t tid2;

/* 函数返回值检查 */
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
    int i;

    printf("i'm thread1 and i will detach myself!\n");
    pthread_detach(pthread_self());        /* 线程 1 脱离自己 */

    for (i = 0;i < 3;i++)    /* 循环打印 3 次信息 */
    {
        printf("thread1 run count: %d\n",i);
        sleep(2);    /* 休眠 2 秒 */
    }

    printf("thread1 exited!\n");
    return NULL;
}

/* 线程 2 入口函数 */
static void* thread2_entry(void* parameter)
{
    int i;

    for (i = 0;i < 3;i++)    /* 循环打印 3 次信息 */
    {
        printf("thread2 run count: %d\n",i);
        sleep(2);    /* 休眠 2 秒 */
    }

    printf("thread2 exited!\n");
    return NULL;
}
/* 用户应用入口 */
int rt_application_init()
{
    int result;

    /* 创建线程 1, 属性为默认值，分离状态为默认值 joinable,
     * 入口函数是 thread1_entry，入口函数参数为 NULL */
    result = pthread_create(&tid1,NULL,thread1_entry,NULL);
    check_result("thread1 created",result);

    /* 创建线程 2, 属性为默认值，分离状态为默认值 joinable,
     * 入口函数是 thread2_entry，入口函数参数为 NULL */
    result = pthread_create(&tid2,NULL,thread2_entry,NULL);
    check_result("thread2 created",result);

    pthread_detach(tid2);    /* 脱离线程 2 */

    return 0;
}
```

### 等待线程结束

```c
int pthread_join (pthread_t thread, void**value_ptr);
```

|    **参数**    |             **描述**                  |
|----------|----------------------|
|   thread   |       线程句柄（线程标识符）         |
| value_ptr | 用户定义的指针，用来存储被等待线程的返回值地址，可由函数 pthread_join() 获取 |
|**返回**| ——          |
| 0         | 成功 |
| EDEADLK  | 线程 join 自己           |
| EINVAL  | join 一个分离状态为 detached 的线程           |
| ESRCH  | 找不到 thread 线程           |

此函数会使调用该函数的线程以阻塞的方式等待线程分离属性为 joinable 的 thread 线程运行结束，并获得 thread 线程的返回值，返回值的地址保存在 value_ptr 里，并释放 thread 线程占用的资源。

pthread_join() 和 pthread_detach() 函数功能类似，都是在线程结束后用来回收线程占用的资源。线程不能等待自己结束，thread 线程的分离状态必须是 joinable，一个线程只对应一次 `pthread_join()` 调用。分离状态为 joinable 的线程仅当有其他线程对其执行了 `pthread_join()` 后，它所占用的资源才会释放。因此为了避免内存泄漏，所有会结束运行的线程，分离状态要么已设为 detached，要么使用 pthread_join() 来回收其占用的资源。

#### 等待线程结束示例代码

以下程序代码会初始化 2 个线程，它们拥有相同的优先级，相同优先级的线程是按照时间片轮转调度。2 个线程属性的分离状态为默认值 joinable，线程 1 先开始运行，循环打印 3 次信息后结束。线程 2 调用 pthread_join() 阻塞等待线程 1 结束，并回收线程 1 占用的资源，然后线程 2 每隔 2 秒钟会打印一次信息。

```c
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>

/* 线程控制块 */
static pthread_t tid1;
static pthread_t tid2;

/* 函数返回值检查 */
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
    int i;

    for (int i = 0;i < 3;i++)    /* 循环打印 3 次信息 */
    {
        printf("thread1 run count: %d\n",i);
        sleep(2);    /* 休眠 2 秒 */
    }

    printf("thread1 exited!\n");
    return NULL;
}

/* 线程 2 入口函数 */
static void* thread2_entry(void* parameter)
{
    int count = 0;
    void* thread1_return_value;

    /* 阻塞等待线程 1 运行结束 */
    pthread_join(tid1, NULL);

    /* 线程 2 打印信息开始输出 */
    while(1)
    {
        /* 打印线程计数值输出 */
        printf("thread2 run count: %d\n",count ++);
        sleep(2);    /* 休眠 2 秒 */
    }

    return NULL;
}

/* 用户应用入口 */
int rt_application_init()
{
    int result;
    /* 创建线程 1, 属性为默认值，分离状态为默认值 joinable,
     * 入口函数是 thread1_entry，入口函数参数为 NULL */
    result = pthread_create(&tid1,NULL,thread1_entry,NULL);
    check_result("thread1 created",result);

    /* 创建线程 2, 属性为默认值，分离状态为默认值 joinable,
     * 入口函数是 thread2_entry，入口函数参数为 NULL */
    result = pthread_create(&tid2,NULL,thread2_entry,NULL);
    check_result("thread2 created",result);

    return 0;
}
```

### 退出线程

```c
void pthread_exit(void *value_ptr);
```

|    **参数**    |           **描述**                  |
|----------|---------------------------|
| value_ptr | 用户定义的指针，用来存储被等待线程的返回值地址，可由函数 pthread_join() 获取 |

pthread 线程调用此函数会终止执行，如同进程调用 exit() 函数一样，并返回一个指向线程返回值的指针。线程退出由线程自身发起。

> [!NOTE]
> 注：若线程的分离状态为 joinable，线程退出后该线程占用的资源并不会被释放，必须调用 pthread_join() 函数释放线程占用的资源。

#### 退出线程示例代码

这个程序会初始化 2 个线程，它们拥有相同的优先级，相同优先级的线程是按照时间片轮转调度。2 个线程属性的分离状态为默认值 joinable，线程 1 先开始运行，打印一次信息后休眠 2 秒，之后打印退出信息然后结束运行。线程 2 调用 pthread_join() 阻塞等待线程 1 结束，并回收线程 1 占用的资源，然后线程 2 每隔 2 秒钟会打印一次信息。

```c
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>

/* 线程控制块 */
static pthread_t tid1;
static pthread_t tid2;

/* 函数返回值核对函数 */
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
    int count = 0;
    while(1)
    {
        /* 打印线程计数值输出 */
        printf("thread1 run count: %d\n",count ++);
        sleep(2);    /* 休眠 2 秒 */
        printf("thread1 will exit!\n");

        pthread_exit(0);    /* 线程 1 主动退出 */
    }
}

/* 线程 2 入口函数 */
static void* thread2_entry(void* parameter)
{
    int count = 0;

    /* 阻塞等待线程 1 运行结束 */
    pthread_join(tid1,NULL);
    /* 线程 2 开始输出打印信息 */
    while(1)
    {
        /* 打印线程计数值输出 */
        printf("thread2 run count: %d\n",count ++);
        sleep(2);    /* 休眠 2 秒 */
    }
}

/* 用户应用入口 */
int rt_application_init()
{
    int result;

    /* 创建线程 1, 属性为默认值，分离状态为默认值 joinable,
     * 入口函数是 thread1_entry，入口函数参数为 NULL */
    result = pthread_create(&tid1,NULL,thread1_entry,NULL);
    check_result("thread1 created",result);

    /* 创建线程 2, 属性为默认值，分离状态为默认值 joinable,
     * 入口函数是 thread2_entry，入口函数参数为 NULL */
    result = pthread_create(&tid2,NULL,thread2_entry,NULL);
    check_result("thread2 created",result);

    return 0;
}
```

## 互斥锁

互斥锁又叫相互排斥的信号量，是一种特殊的二值信号量。互斥锁用来保证共享资源的完整性，保证在任一时刻，只能有一个线程访问该共享资源，线程要访问共享资源，必须先拿到互斥锁，访问完成后需要释放互斥锁。嵌入式的共享资源包括内存、IO、SCI、SPI 等，如果两个线程同时访问共享资源可能会出现问题，因为一个线程可能在另一个线程修改共享资源的过程中使用了该资源，并认为共享资源没有变化。

互斥锁的操作只有两种上锁或解锁，同一时刻只会有一个线程持有某个互斥锁。当有线程持有它时，互斥量处于闭锁状态，由这个线程获得它的所有权。相反，当这个线程释放它时，将对互斥量进行开锁，失去它的所有权。当一个线程持有互斥量时，其他线程将不能够对它进行解锁或持有它。

对互斥锁的主要操作包括：调用 pthread_mutex_init() 初始化一个互斥锁，调用 pthread_mutex_destroy() 销毁互斥锁，调用 pthread_mutex_lock() 对互斥锁上锁，调 pthread_mutex_unlock() 对互斥锁解锁。

使用互斥锁会导致一个潜在问题是线程优先级翻转。在 RT-Thread 操作系统中实现的是优先级继承算法。优先级继承是指，提高某个占有某种资源的低优先级线程的优先级，使之与所有等待该资源的线程中优先级最高的那个线程的优先级相等，然后执行，而当这个低优先级线程释放该资源时，优先级重新回到初始设定。因此，继承优先级的线程避免了系统资源被任何中间优先级的线程抢占。

有关优先级反转的详细介绍请参考[《线程间同步》](../ipc1/ipc1.md) 互斥量小节。

### 互斥锁控制块

每个互斥锁对应一个互斥锁控制块，包含对互斥锁进行的控制的一些信息。创建互斥锁前必须先定义一个 pthread_mutex_t 类型的变量，pthread_mutex_t 是 pthread_mutex 的重定义，pthread_mutex 数据结构定义在 pthread.h 头文件里，数据结构如下：

```c
struct pthread_mutex
{
    pthread_mutexattr_t attr;    /* 互斥锁属性 */
    struct rt_mutex lock;    /* RT-Thread 互斥锁控制块 */
};
typedef struct pthread_mutex pthread_mutex_t;

rt_mutex 是 RT-Thread 内核里定义的一个数据结构，定义在 rtdef.h 头文件里，数据结构如下：

struct rt_mutex
{
    struct rt_ipc_object parent;                /* 继承自 ipc_object 类 */
    rt_uint16_t          value;                  /* 互斥锁的值 */
    rt_uint8_t           original_priority;     /* 持有线程的原始优先级 */
    rt_uint8_t           hold;                    /* 互斥锁持有计数  */
    struct rt_thread    *owner;                 /* 当前拥有互斥锁的线程 */
};
typedef struct rt_mutex* rt_mutex_t;        /* rt_mutext_t 为指向互斥锁结构体的指针 */
```

### 初始化互斥锁

```c
int pthread_mutex_init(pthread_mutex_t *mutex, const pthread_mutexattr_t *attr);
```

|  **参数** |                          **描述**                          |
|----------|------------------------------------------------------|
| mutex |                 互斥锁句柄，不能为 NULL                 |
|  attr | 指向互斥锁属性的指针，若该指针 NULL，则使用默认的属性。 |
|**返回**| ——          |
| 0         | 成功 |
| EINVAL  | 参数无效           |

此函数会初始化 mutex 互斥锁，并根据 attr 指向的互斥锁属性对象设置 mutex 属性，成功初始化后互斥锁处于未上锁状态，线程可以获取，此函数是对 rt_mutex_init() 函数的封装。

除了调用 pthread_mutex_init() 函数创建一个互斥锁，还可以用宏 PTHREAD_MUTEX_INITIALIZER 来静态初始化互斥锁，方法：`pthread_mutex_t mutex =  PTHREAD_MUTEX_INITIALIZER`（结构体常量），等同于调用 pthread_mutex_init() 时 attr 指定为 NULL。

关于互斥锁属性及相关函数会在线程高级编程一章里有详细介绍，一般情况下采用默认属性就可以。

### 销毁互斥锁

```c
int pthread_mutex_destroy(pthread_mutex_t *mutex);
```

|  **参数** |          **描述**          |
|----------|----------------------|
| mutex | 互斥锁句柄，不能为 NULL |
|**返回**| ——          |
| 0         | 成功 |
| EINVAL  | mutex 为空或者 mutex 已经被销毁过     |
| EBUSY  | 互斥锁正在被使用     |

此函数会销毁 mutex 互斥锁。销毁后互斥锁 mutex 处于未初始化状态。销毁以后互斥锁的属性和控制块参数将不在有效，但可以调用 pthread_mutex_init() 对销毁后的互斥锁重新初始化。但不需要销毁使用宏 PTHREAD_MUTEX_INITIALIZER 静态初始化的互斥锁。

当确定互斥锁没有被锁住，且没有线程阻塞在该互斥锁上，才可以销毁该互斥锁。

### 阻塞方式对互斥锁上锁

```c
int pthread_mutex_lock(pthread_mutex_t *mutex);
```

|  **参数** |          **描述**          |
|----------|----------------------|
| mutex | 互斥锁句柄，不能为 NULL |
|**返回**| ——          |
| 0         | 成功 |
| EINVAL  | 参数无效     |
| EDEADLK  | 互斥锁 mutex 不为嵌套锁的情况下线程重复调用此函数     |

此函数对 mutex 互斥锁上锁，此函数是对 rt_mutex_take() 函数的封装。如果互斥锁 mutex 还没有被上锁，那么申请该互斥锁的线程将成功对该互斥锁上锁。如果互斥锁 mutex 已经被当前线程上锁，且互斥锁类型为嵌套锁，则该互斥锁的持有计数加 1，当前线程也不会挂起等待（死锁），但线程必须对应相同次数的解锁。如果互斥锁 mutex 被其他线程上锁持有，则当前线程将被阻塞，一直到其他线程对该互斥锁解锁后，等待该互斥锁的线程将按照先进先出的原则获取互斥锁。

### 非阻塞方式对互斥锁上锁

```c
int pthread_mutex_trylock(pthread_mutex_t *mutex);
```

|  **参数** |          **描述**          |
|----------|----------------------|
| mutex | 互斥锁句柄，不能为 NULL |
|**返回**| ——          |
| 0         | 成功 |
| EINVAL  | 参数无效     |
| EDEADLK  | 互斥锁 mutex 不为嵌套锁的情况下线程重复调用此函数     |
| EBUSY  | 互斥锁 mutex 已经被其他线程上锁     |

此函数是 pthread_mutex_lock() 函数的非阻塞版本。区别在于如果互斥锁 mutex 已经被上锁，线程不会被阻塞，而是马上返回错误码。

### 解锁互斥锁

```c
int pthread_mutex_unlock(pthread_mutex_t *mutex);
```

|  **参数** |          **描述**          |
|----------|----------------------|
| mutex | 互斥锁句柄，不能为 NULL |
|**返回**| ——          |
| 0         | 成功 |
| EINVAL  | 参数无效     |
| EPERM  | 互斥锁 mutex 不为嵌套锁的情况下线程重复调用此函数     |
| EBUSY  | 解锁其他线程持有的类型为检错锁的互斥锁     |

调用此函数给 mutex 互斥锁解锁，是对 rt_mutex_release() 函数的封装。当线程完成共享资源的访问后，应尽快释放占有的互斥锁，使得其他线程能及时获取该互斥锁。只有已经拥有互斥锁的线程才能释放它，每释放一次该互斥锁，它的持有计数就减 1。当该互斥量的持有计数为零时（即持有线程已经释放所有的持有操作），互斥锁才变为可用，等待在该互斥锁上的线程将按先进先出方式被唤醒。如果线程的运行优先级被互斥锁提升，那么当互斥锁被释放后，线程恢复为持有互斥锁前的优先级。

### 互斥锁示例代码

这个程序会初始化 2 个线程，它们拥有相同的优先级，2 个线程都会调用同一个 printer() 函数输出自己的字符串，printer() 函数每次只输出一个字符，之后休眠 1 秒，调用 printer() 函数的线程同样也休眠。如果不使用互斥锁，线程 1 打印了一个字符，休眠后执行线程 2，线程 2 打印一个字符，这样就不能完整的打印线程 1 和线程 2 的字符串，打印出的字符串是混乱的。如果使用了互斥锁保护 2 个线程共享的打印函数 printer()，线程 1 拿到互斥锁后执行 printer() 打印函数打印一个字符，之后休眠 1 秒，这是切换到线程 2，因为互斥锁已经被线程 1 上锁，线程 2 将阻塞，直到线程 1 的字符串打印完整后主动释放互斥锁后线程 2 才会被唤醒。

```c
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>

/* 线程控制块 */
static pthread_t tid1;
static pthread_t tid2;
/* 互斥锁控制块 */
static pthread_mutex_t mutex;
/* 线程共享的打印函数 */
static void printer(char* str)
{
    while(*str != 0)
    {
        putchar(*str);    /* 输出一个字符 */
        str++;
        sleep(1);    /* 休眠 1 秒 */
    }
    printf("\n");
}
/* 函数返回值检查 */
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
/* 线程入口 */
static void* thread1_entry(void* parameter)
{
    char* str = "thread1 hello RT-Thread";
    while (1)
    {
        pthread_mutex_lock(&mutex);      /* 互斥锁上锁 */

        printer(str);  /* 访问共享打印函数 */

        pthread_mutex_unlock(&mutex);  /* 访问完成后解锁 */

        sleep(2);    /* 休眠 2 秒 */
    }
}
static void* thread2_entry(void* parameter)
{
    char* str = "thread2 hi world";
    while (1)
    {
        pthread_mutex_lock(&mutex);  /* 互斥锁上锁 */

        printer(str);  /* 访问共享打印函数 */

        pthread_mutex_unlock(&mutex);  /* 访问完成后解锁 */

        sleep(2);    /* 休眠 2 秒 */
    }
}
/* 用户应用入口 */
int rt_application_init()
{
    int result;
    /* 初始化一个互斥锁 */
    pthread_mutex_init(&mutex,NULL);

    /* 创建线程 1, 线程入口是 thread1_entry, 属性参数为 NULL 选择默认值，入口参数是 NULL*/
    result = pthread_create(&tid1,NULL,thread1_entry,NULL);
    check_result("thread1 created",result);

    /* 创建线程 2, 线程入口是 thread2_entry, 属性参数为 NULL 选择默认值，入口参数是 NULL*/
    result = pthread_create(&tid2,NULL,thread2_entry,NULL);
    check_result("thread2 created",result);

    return 0;
}
```

## 条件变量

条件变量其实就是一个信号量，用于线程间同步。条件变量用来阻塞一个线程，当条件满足时向阻塞的线程发送一个条件，阻塞线程就被唤醒，条件变量需要和互斥锁配合使用，互斥锁用来保护共享数据。

条件变量可以用来通知共享数据状态。比如一个处理共享资源队列的线程发现队列为空，则此线程只能等待，直到有一个节点被添加到队列中，添加后在发一个条件变量信号激活等待线程。

条件变量的主要操作包括：调用 pthread_cond_init() 对条件变量初始化，调用
pthread_cond_destroy() 销毁一个条件变量，调用 pthread_cond_wait() 等待一个条件变量，调用 pthread_cond_signal() 发送一个条件变量。

### 条件变量控制块

每个条件变量对应一个条件变量控制块，包括对条件变量进行操作的一些信息。初始化一个条件变量前需要先定义一个 pthread_cond_t 条件变量控制块。pthread_cond_t 是 pthread_cond 结构体类型的重定义，定义在 pthread.h 头文件里。

```c
struct pthread_cond
{
    pthread_condattr_t attr;         /* 条件变量属性 */
    struct rt_semaphore sem;        /* RT-Thread 信号量控制块 */
};
typedef struct pthread_cond pthread_cond_t;

rt_semaphore 是 RT-Thread 内核里定义的一个数据结构，是信号量控制块，
定义在 rtdef.h 头文件里

struct rt_semaphore
{
   struct rt_ipc_object parent;/* 继承自 ipc_object 类 */
   rt_uint16_t value;   /* 信号量的值  */
};
```

### 初始化条件变量

```c
int pthread_cond_init(pthread_cond_t *cond, const pthread_condattr_t *attr);
```

| **参数** |                       **描述**                       |
|----|------------------------------------------------|
| cond |             条件变量句柄，不能为 NULL             |
| attr | 指向条件变量属性的指针，若为 NULL 则使用默认属性值 |
|**返回**| ——          |
| 0         | 成功 |
| EINVAL  | 参数无效     |

此函数会初始化 cond 条件变量，并根据 attr 指向的条件变量属性设置其属性，此函数是对 rt_sem_init() 函数的一个封装，基于信号量实现。初始化成功后条件变量处于不可用状态。

还可以用宏 PTHREAD_COND_INITIALIZER 静态初始化一个条件变量，方法：
`pthread_cond_t cond = PTHREAD_COND_INITIALIZER`（结构体常量），等同于调用
pthread_cond_init() 时 attr 指定为 NULL。

attr 一般设置 NULL 使用默认值即可，具体会在线程高级编程一章介绍。

### 销毁条件变量

```c
int pthread_cond_destroy(pthread_cond_t *cond);
```

| **参数** |           **描述**           |
|----|------------------------|
| cond | 条件变量句柄，不能为 NULL |
|**返回**| ——          |
| 0         | 成功 |
| EINVAL  | 参数无效     |
| EPERM  | 互斥锁 mutex 不为嵌套锁的情况下线程重复调用此函数     |
| EBUSY  | 条件变量正在被使用     |

此函数会销毁 cond 条件变量，销毁后 cond 处于未初始化状态。销毁之后条件变量的属性及控制块参数将不在有效，但可以调用 pthread_cond_init() 或者静态方式重新初始化。

销毁条件变量前需要确定没有线程被阻塞在该条件变量上，也不会等待获取、发信号或者广播。

### 阻塞方式获取条件变量

```c
int pthread_cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex);
```

|  **参数** |                **描述**                |
|----------|----------------------------------|
|  cond |      条件变量句柄，不能为 NULL      |
| mutex | 指向互斥锁控制块的指针，不能为 NULL |
|**返回**| ——          |
| 0         | 成功 |
| EINVAL  | 参数无效     |

此函数会以阻塞方式获取 cond 条件变量。线程等待条件变量前需要先将 mutex 互斥锁锁住，此函数首先判断条件变量是否可用，如果不可用则初始化一个条件变量，之后解锁 mutex 互斥锁，然后尝试获取一个信号量，当信号量值大于零时，表明信号量可用，线程将获得信号量，也就获得该条件变量，相应的信号量值会减 1。如果信号量的值等于零，表明信号量不可用，线程将阻塞直到信号量可用，之后将对 mutex 互斥锁再次上锁。

### 指定阻塞时间获取条件变量

```c
int pthread_cond_timedwait(pthread_cond_t *cond,
                          pthread_mutex_t *mutex,
                          const struct timespec *abstime);
```

|   **参数**  |                        **描述**                       |
|-------|-------------------------------------------------|
|   cond  |              条件变量句柄，不能为 NULL             |
|  mutex  |         指向互斥锁控制块的指针，不能为 NULL        |
| abstime | 指定的等待时间，单位是操作系统时钟节拍（OS Tick） |
|**返回**| ——          |
| 0         | 成功 |
| EINVAL  | 参数无效     |
| EPERM  | 互斥锁 mutex 不为嵌套锁的情况下线程重复调用此函数     |
| ETIMEDOUT  | 超时     |

此函数和 pthread_cond_wait() 函数唯一的差别在于，如果条件变量不可用，线程将被阻塞 abstime 时长，超时后函数将直接返回 ETIMEDOUT 错误码，线程将会被唤醒进入就绪态。

### 发送满足条件信号量

```c
int pthread_cond_signal(pthread_cond_t *cond);
```

| **参数** |           **描述**           |
|----|------------------------|
| cond | 条件变量句柄，不能为 NULL |
|**返回**| ——          |
| 0         | 成功 |

此函数会发送一个信号且只唤醒一个等待 cond 条件变量的线程，是对 rt_sem_release() 函数的封装，也就是发送一个信号量。当信号量的值等于零，并且有线程等待这个信号量时，将唤醒等待在该信号量线程队列中的第一个线程，由它获取信号量。否则将把信号量的值加 1。

### 广播

```c
int pthread_cond_broadcast(pthread_cond_t *cond);
```

| **参数** |           **描述**           |
|----|------------------------|
| cond | 条件变量句柄，不能为 NULL |
|**返回**| ——          |
| 0         | 成功 |
| EINVAL  | 参数无效     |

调用此函数将唤醒所有等待 cond 条件变量的线程。

### 条件变量示例代码

这个程序是一个生产者消费者模型，有一个生产者线程，一个消费者线程，它们拥有相同的优先级。生产者每隔 2 秒会生产一个数字，放到 head 指向的链表里面，之后调用 pthread_cond_signal() 给消费者线程发信号，通知消费者线程链表里面有数据。消费者线程会调用 pthread_cond_wait() 等待生产者线程发送信号。

```c
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

/* 静态方式初始化一个互斥锁和一个条件变量 */
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

/* 指向线程控制块的指针 */
static pthread_t tid1;
static pthread_t tid2;

/* 函数返回值检查 */
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

/* 生产者生产的结构体数据，存放在链表里 */
struct node
{
    int n_number;
    struct node* n_next;
};
struct node* head = NULL; /* 链表头, 是共享资源 */

/* 消费者线程入口函数 */
static void* consumer(void* parameter)
{
    struct node* p_node = NULL;

    pthread_mutex_lock(&mutex);    /* 对互斥锁上锁 */

    while (1)
    {
        while (head == NULL)    /* 判断链表里是否有元素 */
        {
            pthread_cond_wait(&cond,&mutex); /* 尝试获取条件变量 */
        }
        /*
        pthread_cond_wait() 会先对 mutex 解锁，
        然后阻塞在等待队列，直到获取条件变量被唤醒，
        被唤醒后，该线程会再次对 mutex 上锁，成功进入临界区。
        */

        p_node = head;    /* 拿到资源 */
        head = head->n_next;    /* 头指针指向下一个资源 */
        /* 打印输出 */
        printf("consume %d\n",p_node->n_number);

        free(p_node);    /* 拿到资源后释放节点占用的内存 */
    }
    pthread_mutex_unlock(&mutex);    /* 释放互斥锁 */
    return 0;
}
/* 生产者线程入口函数 */
static void* product(void* patameter)
{
    int count = 0;
    struct node *p_node;

    while(1)
    {
        /* 动态分配一块结构体内存 */
        p_node = (struct node*)malloc(sizeof(struct node));
        if (p_node != NULL)
        {
            p_node->n_number = count++;
            pthread_mutex_lock(&mutex);    /* 需要操作 head 这个临界资源，先加锁 */

            p_node->n_next = head;
            head = p_node;    /* 往链表头插入数据 */

            pthread_mutex_unlock(&mutex);    /* 解锁 */
            printf("produce %d\n",p_node->n_number);

            pthread_cond_signal(&cond);    /* 发信号唤醒一个线程 */

            sleep(2);    /* 休眠 2 秒 */
        }
        else
        {
            printf("product malloc node failed!\n");
            break;
        }
    }
}

int rt_application_init()
{
    int result;

    /* 创建生产者线程, 属性为默认值，入口函数是 product，入口函数参数为 NULL*/
    result = pthread_create(&tid1,NULL,product,NULL);
    check_result("product thread created",result);

    /* 创建消费者线程, 属性为默认值，入口函数是 consumer，入口函数参数是 NULL */
    result = pthread_create(&tid2,NULL,consumer,NULL);
    check_result("consumer thread created",result);

    return 0;
}
```

## 读写锁

读写锁也称为多读者单写者锁。读写锁把对共享资源的访问者划分成读者和写者，读者只对共享资源进行读访问，写者则需要对共享资源进行写操作。同一时间只能有一个线程可以占有写模式的读写锁,
但是可以有多个线程同时占有读模式的读写锁。读写锁适合于对数据结构的读次数比写次数多得多的情况，因为读模式锁定时可以共享,
写模式锁定时意味着独占。

读写锁通常是基于互斥锁和条件变量实现的。一个线程可以对一个读写锁进行多次读写锁定，同样必须有对应次数的解锁。

读写锁的主要操作包括：调用 pthread_rwlock_init() 初始化一个读写锁，写线程调用 pthread_rwlock_wrlock() 对读写锁写锁定，读线程调用 pthread_rwlock_rdlock() 对读写锁读锁定，当不需要使用此读写锁时调用 pthread_rwlock_destroy() 销毁读写锁。

### 读写锁控制块

每个读写锁对应一个读写锁控制块，包括对读写锁进行操作的一些信息。pthread_rwlock_t 是 pthread_rwlock 数据结构的重定义，定义在 pthread.h 头文件里。在创建一个读写锁之前需要先定义一个 pthread_rwlock_t 类型的数据结构。

```c
struct pthread_rwlock
{
    pthread_rwlockattr_t attr;    /* 读写锁属性 */
    pthread_mutex_t rw_mutex;    /* 互斥锁 */
    pthread_cond_t rw_condreaders;    /* 条件变量，供读者线程使用 */
    pthread_cond_t rw_condwriters;    /* 条件变量，供写者线程使用 */
    int rw_nwaitreaders;    /* 读者线程等待计数 */
    int rw_nwaitwriters;    /* 写者线程等待计数 */
    /* 读写锁值，值为 0：未上锁, 值为 - 1：被写者线程锁定, 大于 0 值：被读者线程锁定数量 */
    int rw_refcount;
};
typedef struct pthread_rwlock pthread_rwlock_t;        /* 类型重定义 */
```

### 初始化读写锁初始化

```c
int pthread_rwlock_init (pthread_rwlock_t *rwlock,
                            const pthread_rwlockattr_t *attr);
```

|  **参数**  |                     **描述**                    |
|------|-------------------------------------------|
| rwlock |            读写锁句柄，不能为 NULL           |
|  attr  | 指向读写锁属性的指针，RT-Thread 不使用此变量 |
|**返回**| ——          |
| 0         | 成功 |
| EINVAL  | 参数无效     |

此函数会初始化一个 rwlock 读写锁。此函数使用默认值初始化读写锁控制块的信号量和条件变量，相关计数参数初始为 0 值。初始化后的读写锁处于未上锁状态。

还可以使用宏 PTHREAD_RWLOCK_INITIALIZER 来静态初始化读写锁，方法：
pthread_rwlock_t mutex = PTHREAD_RWLOCK_INITIALIZER（结构体常量），等同于调用pthread_rwlock_init() 时 attr 指定为 NULL。

attr 一般设置 NULL 使用默认值即可，具体会在线程高级编程一章介绍。

### 销毁读写锁

```c
int pthread_rwlock_destroy (pthread_rwlock_t *rwlock);
```

|  **参数**  |          **描述**          |
|------|----------------------|
| rwlock | 读写锁句柄，不能为 NULL |
|**返回**| ——          |
| 0         | 成功 |
| EINVAL  | 参数无效     |
| EBUSY  | 读写锁目前正在被使用或者有线程等待该读写锁     |
| EDEADLK  | 死锁     |

此函数会销毁一个 rwlock 读写锁，对应的会销毁读写锁里的互斥锁和条件变量。销毁之后读写锁的属性及控制块参数将不在有效，但可以调用 pthread_rwlock_init() 或者静态方式重新初始化读写锁。

### 读写锁读锁定

#### 阻塞方式对读写锁读锁定

```c
int pthread_rwlock_rdlock (pthread_rwlock_t *rwlock);
```

|  **参数**  |          **描述**          |
|------|----------------------|
| rwlock | 读写锁句柄，不能为 NULL |
|**返回**| ——          |
| 0         | 成功 |
| EINVAL  | 参数无效     |
| EDEADLK  | 死锁     |

读者线程可以调用此函数来对 rwlock 读写锁进行读锁定。如果读写锁没有被写锁定并且没有写者线程阻塞在该读写锁上，读写线程将成功获取该读写锁。如果读写锁已经被写锁定，读者线程将会阻塞，直到写锁定该读写锁的线程解锁。

#### 非阻塞方式对读写锁读锁定

```c
int pthread_rwlock_tryrdlock (pthread_rwlock_t *rwlock);
```

|  **参数**  |          **描述**          |
|------|----------------------|
| rwlock | 读写锁句柄，不能为 NULL |
|**返回**| ——          |
| 0         | 成功 |
| EINVAL  | 参数无效     |
| EBUSY  | 读写锁目前正在被使用或者有线程等待该读写锁     |
| EDEADLK  | 死锁     |

此函数和 pthread_rwlock_rdlock() 函数的不同在于，如果读写锁已经被写锁定，读者线程不会被阻塞，而是返回一个错误码 EBUSY。

#### 指定阻塞时间对读写锁读锁定

```c
int pthread_rwlock_timedrdlock (pthread_rwlock_t *rwlock,
                                const struct timespec *abstime);
```

|   **参数**  |                        **描述**                       |
|-------|-------------------------------------------------|
|  rwlock |               读写锁句柄，不能为 NULL              |
| abstime | 指定的等待时间，单位是操作系统时钟节拍（OS Tick） |
|**返回**| ——          |
| 0         | 成功 |
| EINVAL  | 参数无效     |
| ETIMEDOUT  | 超时     |
| EDEADLK  | 死锁     |

此函数和 pthread_rwlock_rdlock() 函数的不同在于，如果读写锁已经被写锁定，读者线程将会阻塞指定的 abstime 时长，超时后函数将返回错误码 ETIMEDOUT，线程将会被唤醒进入就绪态。

### 读写锁写锁定

#### 阻塞方式对读写锁写锁定

```c
int pthread_rwlock_wrlock (pthread_rwlock_t *rwlock);
```

|  **参数**  |          **描述**          |
|------|----------------------|
| rwlock | 读写锁句柄，不能为 NULL |
|**返回**| ——          |
| 0         | 成功 |
| EINVAL  | 参数无效     |
| EDEADLK  | 死锁     |

写者线程调用此函数对 rwlock 读写锁进行写锁定。写锁定读写锁类似互斥量，同一时刻只能有一个线程写锁定读写锁。如果没有线程锁定该读写锁，即读写锁值为 0，调用此函数的写者线程将会写锁定读写锁，其他线程此时都不能获取读写锁，如果已经有线程锁定该读写锁，即读写锁值不为 0，则写线程将被阻塞，直到读写锁解锁。

#### 非阻塞方式写锁定读写锁

```c
int pthread_rwlock_trywrlock (pthread_rwlock_t *rwlock);
```

|  **参数**  |          **描述**          |
|------|----------------------|
| rwlock | 读写锁句柄，不能为 NULL |
|**返回**| ——          |
| 0         | 成功 |
| EINVAL  | 参数无效     |
| EBUSY  | 读写锁目前被写锁定或者有写着线程阻塞在该读写锁上     |
| EDEADLK  | 死锁     |

此函数和 pthread_rwlock_wrlock() 函数唯一的不同在于，如果已经有线程锁定该读写锁，即读写锁值不为 0，则调用该函数的写者线程会直接返回一个错误代码，线程不会被阻塞。

#### 指定阻塞时长写锁定读写锁

```c
int pthread_rwlock_timedwrlock (pthread_rwlock_t *rwlock,
                                const struct timespec *abstime);
```

|      **参数**      |            **描述**                    |
|--------------|---------------------|
| rwlock abstime | 读写锁句柄，不能为 NULL 指定的等待时间，单位是操作系统时钟节拍（OS Tick） |
|**返回**| ——          |
| 0         | 成功 |
| EINVAL  | 参数无效     |
| ETIMEDOUT  | 超时     |
| EDEADLK  | 死锁     |

此函数和 pthread_rwlock_wrlock() 函数唯一的不同在于，如果已经有线程锁定该读写锁，即读写锁值不为 0，调用线程阻塞指定的 abstime 时长，超时后函数将返回错误码 ETIMEDOUT，线程将会被唤醒进入就绪态。

### 读写锁解锁

```c
int pthread_rwlock_unlock (pthread_rwlock_t *rwlock);
```

|  **参数**  |          **描述**          |
|------|----------------------|
| rwlock | 读写锁句柄，不能为 NULL |
|**返回**| ——          |
| 0         | 成功 |
| EINVAL  | 参数无效     |
| EDEADLK  | 死锁     |

此函数可以对 rwlock 读写锁解锁。线程对同一个读写锁加锁多次，必须有同样次数的解锁，若解锁后有多个线程等待对读写锁进行锁定，系统将按照先进先出的规则激活等待的线程。

### 读写锁示例代码

这个程序有 2 个读者线程，一个写着线程。2 个读者线程先对读写锁读锁定，之后休眠 2 秒，这是其他的读者线程还是可以对该读写锁读锁定，然后读取共享数据。

```c
#include <pthread.h>
#include <sched.h>
#include <stdio.h>

/* 线程控制块 */
static pthread_t reader1;
static pthread_t reader2;
static pthread_t writer1;
/* 共享数据 book */
static int book = 0;
/* 读写锁 */
static pthread_rwlock_t rwlock;
/* 函数结果检查 */
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
/* 线程入口 */
static void* reader1_entry(void* parameter)
{
    while (1)
    {

        pthread_rwlock_rdlock(&rwlock);  /* 尝试读锁定该读写锁 */

        printf("reader1 read book value is %d\n",book);
        sleep(2);  /* 线程休眠 2 秒，切换到其他线程运行 */

        pthread_rwlock_unlock(&rwlock);  /* 线程运行后对读写锁解锁 */
    }
}
static void* reader2_entry(void* parameter)
{
    while (1)
    {
        pthread_rwlock_rdlock(&rwlock);  /* 尝试读锁定该读写锁 */

        printf("reader2 read book value is %d\n",book);
        sleep(2);  /* 线程休眠 2 秒，切换到其他线程运行 */

        pthread_rwlock_unlock(&rwlock);  /* 线程运行后对读写锁解锁 */
    }
}
static void* writer1_entry(void* parameter)
{
    while (1)
    {
        pthread_rwlock_wrlock(&rwlock);  /* 尝试写锁定该读写锁 */

        book++;
        printf("writer1 write book value is %d\n",book);

        pthread_rwlock_unlock(&rwlock);  /* 对读写锁解锁 */

        sleep(2);  /* 线程休眠 2 秒，切换到其他线程运行 */
    }
}
/* 用户应用入口 */
int rt_application_init()
{
    int result;
    /* 默认属性初始化读写锁 */
    pthread_rwlock_init(&rwlock,NULL);

    /* 创建 reader1 线程, 线程入口是 reader1_entry, 线程属性为默认值，入口参数为 NULL*/
    result = pthread_create(&reader1,NULL,reader1_entry,NULL);
    check_result("reader1 created",result);

    /* 创建 reader2 线程, 线程入口是 reader2_entry, 线程属性为默认值，入口参数为 NULL*/
    result = pthread_create(&reader2,NULL,reader2_entry,NULL);
    check_result("reader2 created",result);

    /* 创建 writer1 线程, 线程入口是 writer1_entry, 线程属性为，入口参数为 NULL*/
    result = pthread_create(&writer1,NULL,writer1_entry,NULL);
    check_result("writer1 created",result);

    return 0;
}
```

## 屏障

屏障是多线程同步的一种方法。barrier 意为屏障或者栏杆，把先后到达的多个线程挡在同一栏杆前，直到所有线程到齐，然后撤下栏杆同时放行。先到达的线程将会阻塞，等到所有调用 pthread_barrier_wait() 函数的线程（数量等于屏障初始化时指定的 count）都到达后，这些线程才会由阻塞状态进入就绪状态再次参与系统调度。

屏障是基于条件变量和互斥锁实现的。主要操作包括：调用 pthread_barrier_init() 初始化一个屏障，其他线程调用 pthread_barrier_wait()，所有线程到期后线程唤醒进入准备状态，屏障不在使用调用 pthread_barrier_destroy() 销毁一个屏障。

### 屏障控制块

创建一个屏障前需要先定义一个 pthread_barrier_t 屏障控制块。pthread_barrier_t 是 pthread_barrier 结构体类型的重定义，定义在 pthread.h 头文件里。

```c
struct pthread_barrier
{
    int count;    /* 指定的等待线程个数 */
    pthread_cond_t cond;        /* 条件变量 */
    pthread_mutex_t mutex;    /* 互斥锁 */
};
typedef struct pthread_barrier pthread_barrier_t;
```

### 创建屏障

```c
int pthread_barrier_init(pthread_barrier_t *barrier,
                        const pthread_barrierattr_t *attr,
                        unsigned count);
```

|   **参数**  |             **描述**                        |
|-------|-------------------------------|
|   attr  | 指向屏障属性的指针，传入 NULL，则使用默认值，非 NULL 必须使用 PTHREAD_PROCESS_PRIVATE |
| barrier |         屏障句柄                    |
|  count  |               指定的等待线程个数 |
|**返回**| ——          |
| 0         | 成功 |
| EINVAL  | 参数无效     |

此函数会创建一个 barrier 屏障，并根据默认的参数对屏障控制块的条件变量和互斥锁初始化，初始化后指定的等待线程个数为 count 个，必须对应 count 个线程调用 pthread_barrier_wait()。

attr 一般设置 NULL 使用默认值即可，具体会在线程高级编程一章介绍。

### 销毁屏障

```c
int pthread_barrier_destroy(pthread_barrier_t *barrier);
```

|   **参数**  |   **描述**   |
|-------|--------|
| barrier | 屏障句柄 |
|**返回**| ——          |
| 0         | 成功 |
| EINVAL  | 参数无效     |

此函数会销毁一个 barrier 屏障。销毁之后屏障的属性及控制块参数将不在有效，但可以调用 pthread_barrier_init() 重新初始化。

### 等待屏障

```c
int pthread_barrier_wait(pthread_barrier_t *barrier);
```

|   **参数**  |   **描述**   |
|-------|--------|
| barrier | 屏障句柄 |
|**返回**| ——          |
| 0         | 成功 |
| EINVAL  | 参数无效     |

此函数同步等待在 barrier 前的线程，由每个线程主动调用，若屏障等待线程个数 count 不为 0，count 将减 1，若减 1 后 count 为 0，表明所有线程都已经到达栏杆前，所有到达的线程将被唤醒重新进入就绪状态，参与系统调度。若减一后 count 不为 0，表明还有线程没有到达屏障，调用的线程将阻塞直到所有线程到达屏障。

### 屏障示例代码

此程序会创建 3 个线程，初始化一个屏障，屏障等待线程数初始化为 3。3 个线程都会调用 pthread_barrier_wait() 等待在屏障前，当 3 个线程都到齐后，3 个线程进入就绪态，之后会每隔 2 秒打印输出计数信息。

```c
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>

/* 线程控制块 */
static pthread_t tid1;
static pthread_t tid2;
static pthread_t tid3;
/* 屏障控制块 */
static pthread_barrier_t barrier;
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
    int count = 0;

    printf("thread1 have arrived the barrier!\n");
    pthread_barrier_wait(&barrier);    /* 到达屏障，并等待其他线程到达 */

    while (1)
    {
        /* 打印线程计数值输出 */
        printf("thread1 count: %d\n",count ++);

        /* 休眠 2 秒 */
        sleep(2);
    }
}
/* 线程 2 入口函数 */
static void* thread2_entry(void* parameter)
{
    int count = 0;

    printf("thread2 have arrived the barrier!\n");
    pthread_barrier_wait(&barrier);

    while (1)
    {
        /* 打印线程计数值输出 */
        printf("thread2 count: %d\n",count ++);

        /* 休眠 2 秒 */
        sleep(2);
    }
}
/* 线程 3 入口函数 */
static void* thread3_entry(void* parameter)
{
    int count = 0;

    printf("thread3 have arrived the barrier!\n");
    pthread_barrier_wait(&barrier);

    while (1)
    {
        /* 打印线程计数值输出 */
        printf("thread3 count: %d\n",count ++);

        /* 休眠 2 秒 */
        sleep(2);
    }
}
/* 用户应用入口 */
int rt_application_init()
{
    int result;
    pthread_barrier_init(&barrier,NULL,3);

    /* 创建线程 1, 线程入口是 thread1_entry, 属性参数设为 NULL 选择默认值，入口参数为 NULL*/
    result = pthread_create(&tid1,NULL,thread1_entry,NULL);
    check_result("thread1 created",result);

    /* 创建线程 2, 线程入口是 thread2_entry, 属性参数设为 NULL 选择默认值，入口参数为 NULL*/
    result = pthread_create(&tid2,NULL,thread2_entry,NULL);
    check_result("thread2 created",result);

    /* 创建线程 3, 线程入口是 thread3_entry, 属性参数设为 NULL 选择默认值，入口参数为 NULL*/
    result = pthread_create(&tid3,NULL,thread3_entry,NULL);
    check_result("thread3 created",result);

}
```
## 线程高级编程

本章节会对一些很少使用的属性对象及相关函数做详细介绍。

RT-Thread 实现的线程属性包括线程栈大小、线程优先级、线程分离状态、线程调度策略。pthread_create() 使用属性对象前必须先对属性对象进行初始化。设置线程属性之类的 API 函数应在创建线程之前就调用。线程属性的变更不会影响到已创建的线程。

线程属性结构 pthread_attr_t 定义在 pthread.h 头文件里。线程属性结构如下：

```c
/* pthread_attr_t 类型重定义 */
typedef struct pthread_attr pthread_attr_t;
/* 线程属性结构体 */
struct pthread_attr
{
    void*      stack_base;        /* 线程栈的地址 */
    rt_uint32_t stack_size;     /* 线程栈大小 */
    rt_uint8_t priority;        /* 线程优先级 */
    rt_uint8_t detachstate;     /* 线程的分离状态 */
    rt_uint8_t policy;          /* 线程调度策略 */
    rt_uint8_t inheritsched;    /* 线程的继承性 */
};
```

#### 线程属性初始化及去初始化

线程属性初始化及去初始化函数如下所示：

```c
int pthread_attr_init(pthread_attr_t *attr);
int pthread_attr_destroy(pthread_attr_t *attr);
```

| **参数** |        **描述**        |
|----|------------------|
| attr | 指向线程属性的指针 |
|**返回**| ——          |
| 0         | 成功 |

使用 pthread_attr_init() 函数会使用默认值初始化线程属性结构体 attr，等同于调用线程初始化函数时将此参数设置为 NULL，使用前需要定义一个 pthread_attr_t 属性对象，此函数必须在 pthread_create() 函数之前调用。

pthread_attr_destroy() 函数对 attr 指向的属性去初始化，之后可以再次调用 pthread_attr_init() 函数对此属性对象重新初始化。

#### 线程的分离状态

设置线程的分离状态 / 获取线程的分离状态如下所示，默认情况下线程是非分离状态。

```c
int pthread_attr_setdetachstate(pthread_attr_t *attr, int state);
int pthread_attr_getdetachstate(pthread_attr_t const *attr, int *state);
```

|  **参数** | **描述**               |
|----------|-------------------|
|  attr | 指向线程属性的指针 |
| state | 线程分离状态       |
|**返回**| ——          |
| 0         | 成功 |

线程分离状态属性值 state 可以是 PTHREAD_CREATE_JOINABL（非分离）和
PTHREAD_CREATE_DETACHED（分离）。

线程的分离状态决定一个线程以什么样的方式来回收自己运行结束后占用的资源。线程的分离状态有 2 种：joinable 或者 detached。当线程创建后，应该调用 pthread_join() 或者 pthread_detach() 回收线程结束运行后占用的资源。如果线程的分离状态为 joinable 其他线程可以调用 pthread_join() 函数等待该线程结束并获取线程返回值，然后回收线程占用的资源。分离状态为 detached 的线程不能被其他的线程所 join，自己运行结束后，马上释放系统资源。

#### 线程的调度策略

设置 \ 获取线程调度策略函数如下所示：

```c
int pthread_attr_setschedpolicy(pthread_attr_t *attr, int policy);
int pthread_attr_getschedpolicy(pthread_attr_t const *attr, int *policy);
```

只实现了函数接口，默认不同优先级基于优先级调度，同一优先级时间片轮询调度

#### 线程的调度参数

设置线程的优先级 / 获取线程的优先级函数如下所示：

```c
int pthread_attr_setschedparam(pthread_attr_t *attr,
                               struct sched_param const *param)；
int pthread_attr_getschedparam(pthread_attr_t const *attr,
                               struct sched_param *param)；
```

|  **参数** |        **描述**        |
|----------|------------------|
|  attr | 指向线程属性的指针 |
| param | 指向调度参数的指针 |
|**返回**| ——          |
| 0         | 成功 |

pthread_attr_setschedparam() 函数设置线程的优先级。使用 param 对线程属性优先级赋值。

**参数** struct sched_param 定义在 sched.h 里，结构如下：

```c
struct sched_param
{
    int sched_priority;    /* 线程优先级 */
};
```

结构体 sched_param 的成员 sched_priority 控制线程的优先级值。

#### 线程的堆栈大小

设置 / 获取 线程的堆栈大小的函数如下所示：

```c
int pthread_attr_setstacksize(pthread_attr_t *attr, size_t stack_size);
int pthread_attr_getstacksize(pthread_attr_t const *attr, size_t *stack_size);
```

|     **参数**    |        **描述**        |
|-----------|------------------|
|     attr    | 指向线程属性的指针 |
| stack_size |    线程堆栈大小    |
|**返回**| ——          |
| 0         | 成功 |

pthread_attr_setstacksize() 函数可以设置堆栈大小，单位是字节。在大多数系统中需要做栈空间地址对齐（例如 ARM 体系结构中需要向 4 字节地址对齐）。

#### 线程堆栈大小和地址

设置 / 获取 线程的堆栈地址和堆栈大小的函数如下所示：

```c
int pthread_attr_setstack(pthread_attr_t *attr,
                          void *stack_base,
                          size_t stack_size);
int pthread_attr_getstack(pthread_attr_t const *attr,
                          void**stack_base,
                          size_t *stack_size);
```

|     **参数**    |        **描述**        |
|-----------|------------------|
|     attr    | 指向线程属性的指针 |
| stack_size |    线程堆栈大小    |
| stack_base |    线程堆栈地址    |
|**返回**| ——          |
| 0         | 成功 |

#### 线程属性相关函数

设置 / 获取线程的作用域的函数如下所示：

```c
int pthread_attr_setscope(pthread_attr_t *attr, int scope)；
int pthread_attr_getscope(pthread_attr_t const *attr);
```

|     **参数**    |        **描述**        |
|-----------|------------------|
|     attr    | 指向线程属性的指针 |
| scope |    线程作用域    |
|**返回**| ——          |
| 0         | scope 为 PTHREAD_SCOPE_SYSTEM |
| EOPNOTSUPP    | scope 为 PTHREAD_SCOPE_PROCESS |
| EINVAL        | scope 为 PTHREAD_SCOPE_SYSTEM |

#### 线程属性示例代码

这个程序会初始化 2 个线程，它们拥有共同的入口函数，但是它们的入口参数不相同。最先创建的线程会使用提供的 attr 线程属性，另外一个线程使用系统默认的属性。线程的优先级是很重要的一个参数，因此这个程序会修改第一个创建的线程的优先级为 8，而系统默认的优先级为 24。

```c
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <sched.h>

/* 线程控制块 */
static pthread_t tid1;
static pthread_t tid2;

/* 函数返回值检查 */
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
/* 线程入口函数 */
static void* thread_entry(void* parameter)
{
    int count = 0;
    int no = (int) parameter; /* 获得线程的入口参数 */

    while (1)
    {
        /* 打印输出线程计数值 */
        printf("thread%d count: %d\n", no, count ++);

        sleep(2);    /* 休眠 2 秒 */
    }
}

/* 用户应用入口 */
int rt_application_init()
{
    int result;
    pthread_attr_t attr;      /* 线程属性 */
    struct sched_param prio;  /* 线程优先级 */

    prio.sched_priority = 8;  /* 优先级设置为 8 */
    pthread_attr_init(&attr);  /* 先使用默认值初始化属性 */
    pthread_attr_setschedparam(&attr,&prio);  /* 修改属性对应的优先级 */

    /* 创建线程 1, 属性为 attr，入口函数是 thread_entry，入口函数参数是 1 */
    result = pthread_create(&tid1,&attr,thread_entry,(void*)1);
    check_result("thread1 created",result);

    /* 创建线程 2, 属性为默认值，入口函数是 thread_entry，入口函数参数是 2 */
    result = pthread_create(&tid2,NULL,thread_entry,(void*)2);
    check_result("thread2 created",result);

    return 0;
}
```

### 线程取消

取消是一种让一个线程可以结束其它线程运行的机制。一个线程可以对另一个线程发送一个取消请求。依据设置的不同，目标线程可能会置之不理，可能会立即结束也可能会将它推迟到下一个取消点才结束。

#### 发送取消请求

可使用如下函数发送取消请求：

```c
int pthread_cancel(pthread_t thread);
```

|  **参数**  |   **描述**   |
|------|--------|
| thread | 线程句柄 |
|**返回**| ——          |
| 0         | 成功 |

此函数发送取消请求给 thread 线程。Thread 线程是否会对取消请求做出回应以及什么时候做出回应依赖于线程取消的状态及类型。

#### 设置取消状态

可使用如下函数设置取消请求：

```c
int pthread_setcancelstate(int state, int *oldstate);
```

|   **参数**   |                  **描述**              |
|--------|-------------------------------|
|   state  | 有两种值：PTHREAD_CANCEL_ENABLE：取消使能 PTHREAD_CANCEL_DISABLE：取消不使能（线程创建时的默认值） |
| oldstate |            保存原来的取消状态      |
|**返回**| ——          |
| 0         | 成功 |
| EINVAL    | state 非 PTHREAD_CANCEL_ENABLE 或者 PTHREAD_CANCEL_DISABLE |

此函数设置取消状态，由线程自己调用。取消使能的线程将会对取消请求做出反应，而取消没有使能的线程不会对取消请求做出反应。

#### 设置取消类型

可使用如下函数设置取消类型，由线程自己调用：

```c
int pthread_setcanceltype(int type, int *oldtype);
```

|   **参数**  |       **描述**                     |
|-------|---------------------------------|
|   type  | 有 2 种值：PTHREAD_CANCEL_DEFFERED：线程收到取消请求后继续运行至下一个取消点再结束。（线程创建时的默认值）PTHREAD_CANCEL_ASYNCHRONOUS：线程立即结束。 |
| oldtype |       保存原来的取消类型                             |
|**返回**| ——          |
| 0         | 成功 |
| EINVAL | state 非 PTHREAD_CANCEL_DEFFERED 或者 PTHREAD_CANCEL_ASYNCHRONOUS |

#### 设置取消点

可使用如下函数设置取消点：

```c
void pthread_testcancel(void);
```

此函数在线程调用的地方创建一个取消点。主要由不包含取消点的线程调用，可以回应取消请求。如果在取消状态处于禁用状态下调用 pthread_testcancel()，则该函数不起作用。

#### 取消点

取消点也就是线程接受取消请求后会结束运行的地方，根据 POSIX 标准，pthread_join()、pthread_testcancel()、pthread_cond_wait()、pthread_cond_timedwait()、sem_wait() 等会引起阻塞的系统调用都是取消点。

RT-Thread 包含的所有取消点如下：

-   mq_receive()

-   mq_send()

-   mq_timedreceive()

-   mq_timedsend()

-   msgrcv()

-   msgsnd()

-   msync()

-   pthread_cond_timedwait()

-   pthread_cond_wait()

-   pthread_join()

-   pthread_testcancel()

-   sem_timedwait()

-   sem_wait()

-   pthread_rwlock_rdlock()

-   pthread_rwlock_timedrdlock()

-   pthread_rwlock_timedwrlock()

-   pthread_rwlock_wrlock()

#### 线程取消示例代码

此程序会创建 2 个线程，线程 2 开始运行后马上休眠 8 秒，线程 1 设置了自己的取消状态和类型，之后在一个无限循环里打印运行计数信息。线程 2 唤醒后向线程 1 发送取消请求，线程 1 收到取消请求后马上结束运行。

```c
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>

/* 线程控制块 */
static pthread_t tid1;
static pthread_t tid2;

/* 函数返回值检查 */
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
    int count = 0;
    /* 设置线程 1 的取消状态使能，取消类型为线程收到取消点后马上结束 */
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);

    while(1)
    {
        /* 打印线程计数值输出 */
        printf("thread1 run count: %d\n",count ++);
        sleep(2);    /* 休眠 2 秒 */
    }
}
/* 线程 2 入口函数 */
static void* thread2_entry(void* parameter)
{
    int count = 0;
    sleep(8);
    /* 向线程 1 发送取消请求 */
    pthread_cancel(tid1);
    /* 阻塞等待线程 1 运行结束 */
    pthread_join(tid1,NULL);
    printf("thread1 exited!\n");
    /* 线程 2 打印信息开始输出 */
    while(1)
    {
        /* 打印线程计数值输出 */
        printf("thread2 run count: %d\n",count ++);
        sleep(2);    /* 休眠 2 秒 */
    }
}
/* 用户应用入口 */
int rt_application_init()
{
    int result;
    /* 创建线程 1, 属性为默认值，分离状态为默认值 joinable,
     入口函数是 thread1_entry，入口函数参数为 NULL */
    result = pthread_create(&tid1,NULL,thread1_entry,NULL);
    check_result("thread1 created",result);

    /* 创建线程 2, 属性为默认值，分离状态为默认值 joinable,
     入口函数是 thread2_entry，入口函数参数为 NULL */
    result = pthread_create(&tid2,NULL,thread2_entry,NULL);
    check_result("thread2 created",result);

    return 0;
}
```

### 一次性初始化

可使用如下函数一次性初始化：

```c
int pthread_once(pthread_once_t * once_control, void (*init_routine) (void));
```

|      **参数**     |   **描述**   |
|-------------|--------|
| once_control | 控制变量 |
| init_routine | 执行函数 |
|**返回**| ——          |
| 0         | 成功 |

有时候我们需要对一些变量只进行一次初始化。如果我们进行多次初始化程序就会出现错误。在传统的顺序编程中，一次性初始化经常通过使用布尔变量来管理。控制变量被静态初始化为 0，而任何依赖于初始化的代码都能测试该变量。如果变量值仍然为 0，则它能实行初始化，然后将变量置为 1。以后检查的代码将跳过初始化。

### 线程结束后清理

线程清理函数接口如下所示：

```c
void pthread_cleanup_pop(int execute);
void pthread_cleanup_push(void (*routine)(void*), void *arg);
```

|   **参数**  |              **描述**             |
|-------|-----------------------------|
| execute | 0 或 1，决定是否执行 cleanup 函数 |
| routine |       指向清理函数的指针      |
|   arg   |      传递给清理函数的参数     |

pthread_cleanup_push() 把指定的清理函数 routine 放到线程的清理函数链表里，  pthread_cleanup_pop() 从清理函数链表头部取出第一项函数，若 execute 为非 0 值，则执行此函数。

### 其他线程相关函数

#### 判断 2 个线程是否相等

```c
int pthread_equal (pthread_t t1, pthread_t t2);
```

|    **参数**    |   **描述**   |
|----------|--------|
| pthread_t | 线程句柄 |
|**返回**| ——          |
| 0         | 不相等 |
| 1         | 相等 |

#### 获取线程句柄

```c
pthread_t pthread_self (void);
```
pthread_self() 返回调用线程的句柄。

#### 获取最大最小优先级

```c
int sched_get_priority_min(int policy);
int sched_get_priority_max(int policy);
```

|  **参数**  |                **描述**               |
|------|---------------------------------|
| policy | 2 个值可选：SCHED_FIFO，SCHED_RR |

sched_get_priority_min() 返回值为 0，RT-Thread 里为最大优先级， sched_get_priority_max() 返回值最小优先级。

### 互斥锁属性

RT-Thread 实现的互斥锁属性包括互斥锁类型和互斥锁作用域。

#### 互斥锁属性初始化及去初始化

```c
int pthread_mutexattr_init(pthread_mutexattr_t *attr);
int pthread_mutexattr_destroy(pthread_mutexattr_t *attr);
```

| **参数** |           **描述**           |
|----|------------------------|
| attr | 指向互斥锁属性对象的指针 |
|**返回**| ——          |
| 0         | 成功 |
| EINVAL        | 参数无效 |

pthread_mutexattr_init() 函数将使用默认值初始化 attr 指向的属性对象，等同于调用 pthread_mutex_init() 函数时将属性参数设置为 NULL。

pthread_mutexattr_destroy() 函数将会对 attr 指向的属性对象去初始化，之后可以调用 pthread_mutexattr_init() 函数重新初始化。

#### 互斥锁作用域

```c
int pthread_mutexattr_setpshared(pthread_mutexattr_t *attr, int  pshared);
int pthread_mutexattr_getpshared(pthread_mutexattr_t *attr, int *pshared);
```

|   **参数**  |    **描述**           |
|-------|--------------------|
|   type  |    互斥锁类型               |
| pshared | 有 2 个可选值: PTHREAD_PROCESS_PRIVATE：默认值，用于仅同步该进程中的线程。PTHREAD_PROCESS_SHARED：用于同步该进程和其他进程中的线程。 |
|**返回**| ——          |
| 0         | 成功 |
| EINVAL        | 参数无效 |

#### 互斥锁类型

```c
int pthread_mutexattr_settype(pthread_mutexattr_t *attr, int type);
int pthread_mutexattr_gettype(const pthread_mutexattr_t *attr, int *type);
```

| **参数** |           **描述**           |
|----|------------------------|
| type |        互斥锁类型        |
| attr | 指向互斥锁属性对象的指针 |
|**返回**| ——          |
| 0         | 成功 |
| EINVAL        | 参数无效 |

互斥锁的类型决定了一个线程在获取一个互斥锁时的表现方式，RT-Thread 实现了 3 种互斥锁类型：

-   PTHREAD_MUTEX_NORMAL：普通锁，当一个线程加锁以后，其余请求锁的线程将形成一个等待队列，并在解锁后按先进先出方式获得锁。如果一个线程在不首先解除互斥锁的情况下尝试重新获得该互斥锁，不会产生死锁，而是返回错误码，和检错锁一样。

-   PTHREAD_MUTEX_RECURSIVE：嵌套锁，允许一个线程对同一个锁成功获得多次，需要相同次数的解锁释放该互斥锁。

-   PTHREAD_MUTEX_ERRORCHECK：检错锁，如果一个线程在不首先解除互斥锁的情况下尝试重新获得该互斥锁，则返回错误。这样就保证当不允许多次加锁时不会出现死锁。

### 条件变量属性

使用默认值 PTHREAD_PROCESS_PRIVATE 初始化条件变量属性 attr 可使用如下函数：

```c
int pthread_condattr_init(pthread_condattr_t *attr);
```

| **参数** |            **描述**            |
|----|--------------------------|
| attr | 指向条件变量属性对象的指针 |
|**返回**| ——          |
| 0         | 成功 |
| EINVAL        | 参数无效 |

#### 获取条件变量作用域

```c
int pthread_mutexattr_getpshared(pthread_mutexattr_t *attr, int *pshared);
```

| **参数** |            **描述**            |
|----|--------------------------|
| attr | 指向条件变量属性对象的指针 |
|**返回**| ——          |
| 0         | 成功 |
| EINVAL        | 参数无效 |

### 读写锁属性

#### 初始化属性

```c
int pthread_rwlockattr_init (pthread_rwlockattr_t *attr);
```

| **参数** |         **描述**         |
|----|--------------------|
| attr | 指向读写锁属性的指针 |
|**返回**| ——          |
| 0         | 成功 |
|-1         | 参数无效 |

该函数会使用默认值 PTHREAD_PROCESS_PRIVATE 初始化读写锁属性 attr。

#### 获取作用域

```c
int pthread_rwlockattr_getpshared (const pthread_rwlockattr_t *attr, int *pshared);
```

|   **参数**  |            **描述**            |
|-------|--------------------------|
|   attr  |    指向读写锁属性的指针    |
| pshared | 指向保存读写锁作用域的指针 |
|**返回**| ——          |
| 0         | 成功 |
|-1         | 参数无效 |

pshared 指向的内存保存的值为 PTHREAD_PROCESS_PRIVATE。

### 屏障属性

#### 初始化属性

```c
int pthread_barrierattr_init(pthread_barrierattr_t *attr);
```

| **参数** |        **描述**        |
|----|------------------|
| attr | 指向屏障属性的指针 |
|**返回**| ——          |
| 0         | 成功 |
|-1         | 参数无效 |

改函数会使用默认值 PTHREAD_PROCESS_PRIVATE 初始化屏障属性 attr。

#### 获取作用域

```c
int pthread_barrierattr_getpshared(const pthread_barrierattr_t *attr, int *pshared);
```

|   **参数**  | **描述**                         |
|-------|-----------------------------|
|   attr  | 指向屏障属性的指针           |
| pshared | 指向保存屏障作用域数据的指针 |
|**返回**| ——          |
| 0         | 成功 |
|-1         | 参数无效 |

### 消息队列属性

消息队列属性控制块如下：

```c
struct mq_attr
{
    long mq_flags;      /* 消息队列的标志，用来表示是否阻塞 */
    long mq_maxmsg;     /* 消息队列最大消息数 */
    long mq_msgsize;    /* 消息队列每个消息的最大字节数 */
    long mq_curmsgs;    /* 消息队列当前消息数 */
};
```
#### 获取属性
```c
int mq_getattr(mqd_t mqdes, struct mq_attr *mqstat);
```

|  **参数**  |           **描述**           |
|------|------------------------|
|  mqdes | 指向消息队列控制块的指针 |
| mqstat |  指向保存获取数据的指针  |
|**返回**| ——          |
| 0         | 成功 |
|-1         | 参数无效 |
