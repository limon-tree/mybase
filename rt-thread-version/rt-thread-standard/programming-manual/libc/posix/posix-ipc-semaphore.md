## 信号量

信号量可以用于进程与进程之间，或者进程内线程之间的通信。每个信号量都有一个不会小于 0 的信号量值，对应信号量的可用数量。调用 sem_init() 或者 sem_open() 给信号量值赋初值，调用 sem_post() 函数可以让信号量值加 1，调用 sem_wait() 可以让信号量值减 1，如果当前信号量为 0，调用 sem_wait() 的线程被挂起在该信号量的等待队列上，直到信号量值大于 0，处于可用状态。

根据信号量的值（代表可用资源的数目）的不同，POSIX 信号量可以分为：

-   二值信号量：信号量的值只有 0 和 1，初始值指定为 1。这和互斥锁一样，若资源被锁住，信号量的值为 0，若资源可用，则信号量的值为 1。相当于只有一把钥匙，线程拿到钥匙后，完成了对共享资源的访问后需要解锁，把钥匙再放回去，给其他需要此钥匙的线程使用。使用方法和互斥锁一样，等待信号量函数必须和发送信号量函数成对使用，不能单独使用，必须先等待后发送。

-   计数信号量：信号量的值在 0 到一个大于 1 的限制值（POSIX 指出系统的最大限制值至少要为 32767）。该计数表示可用信号量个数。此时，发送信号量函数可以被单独调用发送信号量，相当于有多把钥匙，线程拿到一把钥匙就消耗了一把，使用过的钥匙不必在放回去。

POSIX 信号量又分为有名信号量和无名信号量：

-   有名信号量：其值保存在文件中，一般用于进程间同步或互斥。

-   无名信号量：其值保存在内存中，一般用于线程间同步或互斥。

RT-Thread 操作系统的 POSIX 信号量主要是基于 RT-Thread 内核信号量的一个封装，主要还是用于系统内线程间的通讯。使用方式和 RT-Thread 内核的信号量差不多。

### 信号量控制块

每个信号量对应一个信号量控制块，创建一个信号量前需要先定义一个 sem_t 信号量控制块。sem_t 是 posix_sem 结构体类型的重定义，定义在 semaphore.h 头文件里。

```c
struct posix_sem
{
    rt_uint16_t refcount;
    rt_uint8_t unlinked;
    rt_uint8_t unamed;
    rt_sem_t sem;    /* RT-Thread 信号量 */
    struct posix_sem* next;     /* 指向下一个信号量控制块 */
};
typedef struct posix_sem sem_t;

rt_sem_t 是 RT-Thread 信号量控制块，定义在 rtdef.h 头文件里。

struct rt_semaphore
{
   struct rt_ipc_object parent;/* 继承自 ipc_object 类 */
   rt_uint16_t value;   /* 信号量的值  */
};
/* rt_sem_t 是指向 semaphore 结构体的指针类型 */
typedef struct rt_semaphore* rt_sem_t;

```

### 无名信号量

无名信号量的值保存在内存中，一般用于线程间同步或互斥。在使用之前，必须先调用 sem_init() 初始化。

#### 初始化无名信号量

```c
int sem_init(sem_t *sem, int pshared, unsigned int value);
```

|   **参数**  |                  **描述**                  |
|-------|--------------------------------------|
|   sem   |               信号量句柄               |
|  value  | 信号量初始值，表示信号量资源的可用数量 |
| pshared |           RT-Thread 未实现参数          |
|**返回**| ——          |
| 0         | 成功 |
| -1  | 失败     |

此函数初始化一个无名信号量 sem，根据给定的或默认的参数对信号量相关数据结构进行初始化，并把信号量放入信号量链表里。初始化后信号量值为给定的初始值 value。此函数是对 rt_sem_create() 函数的封装。

#### 销毁无名信号量

```c
int sem_destroy(sem_t *sem);
```

| **参数** |    **描述**    |
|----|----------|
|  sem | 信号量句柄 |
|**返回**| ——          |
| 0         | 成功 |
| -1        | 失败     |

此函数会销毁一个无名信号量 sem，并释放信号量占用的资源。

### 有名信号量

有名信号量，其值保存在文件中，一般用于进程间同步或互斥。两个进程可以操作相同名称的有名信号量。RT-Thread 操作系统中的有名信号量实现和无名信号量差不多，都是设计用于线程间的通信，使用方法也类似。

#### 创建或打开有名信号量

```c
sem_t *sem_open(const char *name, int oflag, ...);
```

|  **参数** |       **描述**       |
|----------|----------------|
|  name |    信号量名称    |
| oflag | 信号量的打开方式 |
|**返回**| ——          |
| 信号量句柄         | 成功 |
| NULL          | 失败     |

此函数会根据信号量名字 name 创建一个新的信号量或者打开一个已经存在的信号量。Oflag 的可选值有 0、O_CREAT 或 O_CREAT\|O_EXCL。如果 Oflag 设置为 O_CREAT 则会创建一个新的信号量。如果 Oflag 设置 O_CREAT\|O_EXCL，如果信号量已经存在则会返回 NULL，如果不存在则会创建一个新的信号量。如果 Oflag 设置为 0，信号量不存在则会返回 NULL。

#### 分离有名信号量

```c
int sem_unlink(const char *name);
```

| **参数** |    **描述**    |
|----|----------|
| name | 信号量名称 |
|**返回**| ——          |
| 0         | 成功 |
| -1        | 失败，信号量不存在     |

此函数会根据信号量名称 name 查找该信号量，若信号量存在，则将该信号量标记为分离状态。之后检查引用计数，若值为 0，则立即删除信号量，若值不为 0，则等到所有持有该信号量的线程关闭信号量之后才会删除。

#### 关闭有名信号量

```c
int sem_close(sem_t *sem);
```

| **参数** |    **描述**    |
|----|----------|
|  sem | 信号量句柄 |
|**返回**| ——          |
| 0         | 成功 |
| -1        | 失败    |

当一个线程终止时，会对其占用的信号量执行此关闭操作。不论线程是自愿终止还是非自愿终止都会执行这个关闭操作，相当于是信号量的持有计数减 1。若减 1 后持有计数为 0 且信号量已经处于分离状态，则会删除 sem 信号量并释放其占有的资源。

### 获取信号量值

```c
int sem_getvalue(sem_t *sem, int *sval);
```

| **参数** |                **描述**               |
|----|---------------------------------|
|  sem |       信号量句柄，不能为 NULL      |
| sval | 保存获取的信号量值地址, 不能为 NULL |
|**返回**| ——          |
| 0         | 成功 |
| -1        | 失败    |

此函数可以获取 sem 信号量的值，并保存在 sval 指向的内存里，可以知道信号量的资源数量。

### 阻塞方式等待信号量

```c
int sem_wait(sem_t *sem);
```

| **参数** |          **描述**          |
|----|----------------------|
|  sem | 信号量句柄，不能为 NULL |
|**返回**| ——          |
| 0         | 成功 |
| -1        | 失败    |

线程调用此函数获取信号量，是 rt_sem_take(sem，RT_WAITING_FOREVER) 函数的封装。若信号量值大于零，表明信号量可用，线程获得信号量，信号量值减 1。若信号量值等于 0，表明信号量不可用，线程阻塞进入挂起状态，并按照先进先出的方式排队等待，直到信号量可用。

### 非阻塞方式获取信号量

```c
int sem_trywait(sem_t *sem);
```

| **参数** |          **描述**          |
|----|----------------------|
|  sem | 信号量句柄，不能为 NULL |
|**返回**| ——          |
| 0         | 成功 |
| -1        | 失败    |

此函数是 sem_wait() 函数的非阻塞版，是 rt_sem_take(sem,0) 函数的封装。当信号量不可用时，线程不会阻塞，而是直接返回。

### 指定阻塞时间等待信号量

```c
int sem_timedwait(sem_t *sem, const struct timespec *abs_timeout);
```

|     **参数**     |                        **描述**                       |
|------------|-------------------------------------------------|
|      sem     |               信号量句柄，不能为 NULL              |
| abs_timeout | 指定的等待时间，单位是操作系统时钟节拍（OS Tick） |
|**返回**| ——          |
| 0         | 成功 |
| -1        | 失败    |

此函数和 sem_wait() 函数的区别在于，若信号量不可用，线程将阻塞 abs_timeout 时长，超时后函数返回 - 1，线程将被唤醒由阻塞态进入就绪态。

### 发送信号量

```c
int sem_post(sem_t *sem);
```

| **参数** |          **描述**          |
|----|----------------------|
|  sem | 信号量句柄，不能为 NULL |
|**返回**| ——          |
| 0         | 成功 |
| -1        | 失败    |

此函数将释放一个 sem 信号量，是 rt_sem_release() 函数的封装。若等待该信号量的线程队列不为空，表明有线程在等待该信号量，第一个等待该信号量的线程将由挂起状态切换到就绪状态，等待系统调度。若没有线程等待该信号量，该信号量值将加 1。

### 无名信号量使用示例代码

信号量使用的典型案例是生产者消费者模型。一个生产者线程和一个消费者线程对同一块内存进行操作，生产者往共享内存填充数据，消费者从共享内存读取数据。

此程序会创建 2 个线程，2 个信号量，一个信号量表示共享数据为空状态，一个信号量表示共享数据不为空状态，一个互斥锁用于保护共享资源。生产者线程生产好数据后会给消费者发送一个 full_sem 信号量，通知消费者线程有数据可用，休眠 2 秒后会等待消费者线程发送的 empty_sem 信号量。消费者线程等到生产者发送的 full_sem 后会处理共享数据，处理完后会给生产者线程发送 empty_sem 信号量。程序会这样一直循环。

```c
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>

/* 静态方式初始化一个互斥锁用于保护共享资源 */
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
/* 2 个信号量控制块，一个表示资源空信号，一个表示资源满信号 */
static sem_t empty_sem,full_sem;

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

    while (1)
    {
        sem_wait(&full_sem);
        pthread_mutex_lock(&mutex);    /* 对互斥锁上锁, */

        while (head != NULL)    /* 判断链表里是否有元素 */
        {
            p_node = head;    /* 拿到资源 */
            head = head->n_next;    /* 头指针指向下一个资源 */
            /* 打印输出 */
            printf("consume %d\n",p_node->n_number);

            free(p_node);    /* 拿到资源后释放节点占用的内存 */
        }

        pthread_mutex_unlock(&mutex);    /* 临界区数据操作完毕，释放互斥锁 */

        sem_post(&empty_sem);  /* 发送一个空信号量给生产者 */
    }
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

            sem_post(&full_sem);  /* 发送一个满信号量给消费者 */
        }
        else
        {
            printf("product malloc node failed!\n");
            break;
        }
        sleep(2);    /* 休眠 2 秒 */
        sem_wait(&empty_sem);  /* 等待消费者发送空信号量 */
    }
}

int rt_application_init()
{
    int result;

    sem_init(&empty_sem,NULL,0);
    sem_init(&full_sem,NULL,0);
    /* 创建生产者线程, 属性为默认值，入口函数是 product，入口函数参数为 NULL*/
    result = pthread_create(&tid1,NULL,product,NULL);
    check_result("product thread created",result);

    /* 创建消费者线程, 属性为默认值，入口函数是 consumer，入口函数参数是 NULL */
    result = pthread_create(&tid2,NULL,consumer,NULL);
    check_result("consumer thread created",result);

    return 0;
}
```
