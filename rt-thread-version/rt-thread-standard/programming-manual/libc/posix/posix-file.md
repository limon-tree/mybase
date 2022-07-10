## 文件系统函数

本节介绍对文件进行操作的相关函数，对文件的操作一般都要基于文件描述符 fd，如下图所示：

![文件管理常用函数](../../filesystem/figures/fs-mg.png)

具体的文件系统函数已经在[虚拟文件系统](../../filesystem/filesystem.md)中实现，可以点此[跳转](../../filesystem/filesystem.md)

## select 函数

### select 函数原型：

```c
int select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds，struct timeval *timeout);
```

#### 【参数说明】

* **nfds**：select监视的文件句柄数，一般设为要监视各文件中的最大文件描述符值加1。
* **readfds**：文件描述符集合监视文件集中的任何文件是否有数据可读，当select函数返回的时候，readfds将清除其中不可读的文件描述符，只留下可读的文件描述符。
* **writefds**：文件描述符集合监视文件集中的任何文件是否有数据可写，当select函数返回的时候，writefds将清除其中不可写的文件描述符，只留下可写的文件描述符。
* **exceptfds**：文件集将监视文件集中的任何文件是否发生错误，可用于其他的用途，例如，监视带外数据OOB，带外数据使用MSG\_OOB标志发送到套接字上。当select函数返回的时候，exceptfds将清除其中的其他文件描述符，只留下标记有OOB数据的文件描述符。
* **timeout** 参数是一个指向 struct timeval 类型的指针，它可以使 select\(\)在等待 timeout 时间后若没有文件描述符准备好则返回。其timeval结构用于指定这段时间的秒数和微秒数。它可以使select处于三种状态：

> \(1\) 若将NULL以形参传入，即不传入时间结构，就是将select置于阻塞状态，一定等到监视文件描述符集合中某个文件描述符发生变化为止；
> \(2\) 若将时间值设为0秒0毫秒，就变成一个纯粹的非阻塞函数，不管文件描述符是否有变化，都立刻返回继续执行，文件无变化返回0，有变化返回一个正值；
> \(3\) timeout的值大于0，这就是等待的超时时间，即select在timeout时间内阻塞，超时时间之内有事件到来就返回了，否则在超时后不管怎样一定返回，返回值同上述。

timeval 结构体定义

```c
struct timeval
{
    int tv_sec;      /* 秒 */
    int tv_usec;     /* 微妙 */
};
```

#### 【返回值】

* **int**：若有就绪描述符返回其数目，若超时则为0，若出错则为-1

下列操作用来设置、清除、判断文件描述符集合。

```c
FD_ZERO(fd_set *set);           // 清除一个文件描述符集。
FD_SET(int fd,fd_set *set);     // 将一个文件描述符加入文件描述符集中。
FD_CLR(int fd,fd_set *set);     // 将一个文件描述符从文件描述符集中清除。
FD_ISSET(int fd,fd_set *set);   // 判断文件描述符是否被置位
```

fd\_set可以理解为一个集合，这个集合中存放的是文件描述符\(file descriptor\)，即文件句柄。中间的三个参数指定我们要让内核测试读、写和异常条件的文件描述符集合。如果对某一个的条件不感兴趣，就可以把它设为空指针。

**select\(\)的机制中提供一种fd\_set的数据结构**，实际上是一个long类型的数组，每一个数组元素都能与打开的文件句柄（不管是Socket句柄，还是其他文件或命名管道或设备句柄）建立联系，建立联系的工作由程序员完成，当调用select\(\)时，由内核根据IO状态修改fd\_set的内容，由此来通知执行了select\(\)的进程哪一Socket或文件可读。

## poll 函数

### poll 的函数原型：

```c
int poll(struct pollfd *fds, nfds_t nfds, int timeout);
```

#### 【参数说明】

* **fds**：fds是一个struct pollfd类型的数组，用于存放需要检测其状态的socket描述符，并且调用poll函数之后fds数组不会被清空；一个pollfd结构体表示一个被监视的文件描述符，通过传递fds指示 poll\(\) 监视多个文件描述符。

struct pollfd原型如下：

```c
typedef struct pollfd {
        int fd;                 // 需要被检测或选择的文件描述符
        short events;           // 对文件描述符fd上感兴趣的事件
        short revents;          // 文件描述符fd上当前实际发生的事件
} pollfd_t;
```

其中，结构体的events域是监视该文件描述符的事件掩码，由用户来设置这个域，结构体的revents域是文件描述符的操作结果事件掩码，内核在调用返回时设置这个域。

* **nfds**：记录数组fds中描述符的总数量。
* **timeout**：指定等待的毫秒数，无论 I/O 是否准备好，poll\(\) 都会返回，和select函数是类似的。

#### 【返回值】

* **int**：函数返回fds集合中就绪的读、写，或出错的描述符数量，返回0表示超时，返回-1表示出错；

poll改变了文件描述符集合的描述方式，使用了pollfd结构而不是select的fd\_set结构，使得poll支持的文件描述符集合限制远大于select的1024。这也是和select不同的地方。

## 标准 I/O 接口函数

如果需要读写标准输入输出(标准 I/O 接口)，调用以下函数/功能，请开启 `RT_USING_POSIX_FS` 和 `RT_USING_POSIX_STDIO` 宏。若读写文件系统中的文件，则仅需开启 `RT_USING_POSIX_FS`，无需开启 `RT_USING_POSIX_STDIO`。

```c
/* 标准输出 */
write(STDOUT_FILENO 或 STDERR_FILENO);
printf(...); /* 该函数仅需在gcc下使能上述两个宏，在其他编译器下，可以直接使用 */
fprintf(stdout 或 stderr);
fwrite(stdout 或 stderr);
fputs(stdout 或 stderr);
fputc(stdout 或 stderr);
puts();

/* 标准输入 */
getchar();
read(STDIN_FILENO);
fread(stdin);
fgetc(stdin);
fgets(stdin);
```

> 注：如果已经开启 FinSH 功能的话，可以在 FinSH 线程下，使用 finsh_getchar 代替 getchar，来获取从终端键入的字符。

