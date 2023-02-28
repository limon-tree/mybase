
# frr 的基本数据结构 内存管理



## 1 quagga的memory

 1 quagga的memory 比较容易阅读,我们先从quagga入手,quagga的memory是对原malloc,calloc,free等的一种包裹(wrap),内部对分配的内存进行记录(log),分析,分类,已达到对内存简单管理的目的

```c
/* #define MEMORY_LOG */
#ifdef MEMORY_LOG
#define XMALLOC(mtype, size) \
  mtype_zmalloc (__FILE__, __LINE__, (mtype), (size))
#define XCALLOC(mtype, size) \
  mtype_zcalloc (__FILE__, __LINE__, (mtype), (size))
#define XREALLOC(mtype, ptr, size)  \
  mtype_zrealloc (__FILE__, __LINE__, (mtype), (ptr), (size))
#define XFREE(mtype, ptr) \
  do { \
    mtype_zfree (__FILE__, __LINE__, (mtype), (ptr)); \
    ptr = NULL; } \
  while (0)
#define XSTRDUP(mtype, str) \
  mtype_zstrdup (__FILE__, __LINE__, (mtype), (str))
#else
#define XMALLOC(mtype, size)       zmalloc ((mtype), (size))
#define XCALLOC(mtype, size)       zcalloc ((mtype), (size))
#define XREALLOC(mtype, ptr, size) zrealloc ((mtype), (ptr), (size))
#define XFREE(mtype, ptr)          do { \
                                     zfree ((mtype), (ptr)); \
                                     ptr = NULL; } \
                                   while (0)
#define XSTRDUP(mtype, str)        zstrdup ((mtype), (str))
#endif /* MEMORY_LOG */
```

2.如果 MEMORY_LOG定义,xmalloc->mtype_zmalloc,mstat记录malloc的次数,并调用mtype_log记录malloc的函数名,地址,文件,行数,zmalloc->malloc,记录那种类型alloc++(alloc_inc)

```c
void *
mtype_zmalloc(const char *file, int line, int type, size_t size)
{
    void *memory;

    mstat[type].c_malloc++;
    mstat[type].t_malloc++;

    memory = zmalloc(type, size);
    mtype_log("zmalloc", memory, file, line, type);

    return memory;
}
static void
mtype_log(char *func, void *memory, const char *file, int line, int type)
{
    zlog_debug("%s: %s %p %s %d", func, lookup(mstr, type), memory, file, line);
}
void *
zmalloc(int type, size_t size)
{
    void *memory;

    memory = malloc(size);

    memset(memory, 0, sizeof(size));
    if (memory == NULL)
        zerror("malloc", type, size);

    alloc_inc(type);

    return memory;
}
```

3.如果没有定义MEMORY_LOG,则直接调用zmalloc,记录那种类型alloc++(alloc_inc),由此可见打开MEMORY_LOG开关,可以对那个模块的内存释放分析,看是否有内存泄漏.

4.内存的记录信息 的结构体数组`static struct mstat[] `  如下:

```c
#ifdef MEMORY_LOG
static struct
{
    const char *name;
    long alloc;
    unsigned long t_malloc;
    unsigned long c_malloc;
    unsigned long t_calloc;
    unsigned long c_calloc;
    unsigned long t_realloc;
    unsigned long t_free;
    unsigned long c_strdup;
} mstat [MTYPE_MAX];
#else
static struct
{
    char *name;
    long alloc;
} mstat [MTYPE_MAX];
#endif /* MEMORY_LOG */
enum
{
  MTYPE_TMP = 1,
  MTYPE_STRVEC,
  MTYPE_VECTOR,
  MTYPE_VECTOR_INDEX,
  MTYPE_LINK_LIST,
  MTYPE_LINK_NODE,
    ...
  MTYPE_MAX,
}

struct mlist mlists[] __attribute__ ((unused)) = {
  { memory_list_lib,	"LIB"	},
  { memory_list_zebra,	"ZEBRA"	},
}
struct memory_list
{
  int index;
  const char *format;
};
struct memory_list memory_list_lib[] =
{
  { MTYPE_TMP,			"Temporary memory"		},
  { MTYPE_STRVEC,		"String vector"			},
  { MTYPE_VECTOR,		"Vector"			},
  { MTYPE_VECTOR_INDEX,		"Vector index"			},
  { MTYPE_LINK_LIST,		"Link List"			},
  { MTYPE_LINK_NODE,		"Link Node"			},
  { -1, NULL },
}
```

5. 以比较常用的MTYPE_TMP分析, temp的内存信息都保存在mlists中 `show memory` 可以查看

```c
DEFUN(show_memory,
      show_memory_cmd,
      "show memory",
      "Show running system information\n"
      "Memory statistics\n")
{
    struct mlist *ml;
    int needsep = 0;

#ifdef HAVE_MALLINFO
    needsep = show_memory_mallinfo(vty);
#endif /* HAVE_MALLINFO */

    for (ml = mlists; ml->list; ml++)
    {
        if (needsep)
            show_separator(vty);
        needsep = show_memory_vty(vty, ml->list); //vty_out(vty, "%-30s: %10ld\r\n", m->format, mstat[m->index].alloc);
    }

    return CMD_SUCCESS;
}

show memory
Memory statistics for zebra:		  alloc
Temporary memory              :          7
String vector                 :          2
Vector                        :        822
Vector index                  :        822
Link List                     :         22
Link Node                     :         16
Thread                        :          5
Thread master                 :          1
Thread stats                  :          3
VTY                           :          2
Interface                     :          6
Connected                     :          6
Buffer                        :          1
Buffer data                   :          1
Prefix                        :          7
Hash                          :          1
Hash Bucket                   :          3
Hash Index                    :          1
Route table                   :         15
Route node                    :         15
Command desc                  :       9189
Logging                       :          1
    

```

## 2 frr的memory







```bash
iHSHN-200# show memory
Memory statistics for zebra:
System allocator statistics:
  Total heap allocated:  2792 KiB
  Holding block headers: 0 bytes
  Used small blocks:     0 bytes
  Used ordinary blocks:  2544 KiB
  Free small blocks:     1152 bytes
  Free ordinary blocks:  248 KiB
  Ordinary blocks:       3
  Small blocks:          64
  Holding blocks:        0
(see system documentation for 'mallinfo' for meaning)
--- qmem logging subsystem ---
Type                          : Current#   Size       Total     Max#  MaxBytes
syslog target                 :        1     28          28        1        28
--- qmem libfrr ---
Type                          : Current#   Size       Total     Max#  MaxBytes
log thread-local buffer       :        3   5644       16932        3     16932
YANG data structure           :        0   1028           0        1      1036
YANG module                   :        6     24         168        6       168
Work queue name string        :        1     22          28        1        28
Work queue item               :        0     12           0        1        12
Work queue                    :        2 variable       136        2       136
VTY                           :        7 variable     90612        7     90612
VRF                           :        1    132         140        1       140
Vector index                  :     9633 variable    158652     9640    158780
Vector                        :     9633     12      115596     9640    115680
Typed-heap array              :        0    288           0        1       300
Typed-hash bucket             :        7 variable       228        7       228
Thread stats                  :       14     36         616       14       616
Thread Poll Info              :        6   4096       24648        6     24648
Thread master                 :       11 variable     49892       11     49892
Thread                        :       15     88        1380       15      1380
Route node                    :       22 variable      2024       22      2024
Route table                   :       15     32         660       15       660
Stream FIFO                   :        1     36          44        1        44
Route map name                :        0      4           0        1        12
Privilege information         :        4 variable        96        4        96
Prefix                        :        3     48         180        3       180
Northbound Configuration Entry:        4   1028        4144        4      4144
Northbound Configuration      :        2      8          24        2        24
Northbound Node               :      648   1104      723168      648    723168
Nexthop Group                 :        0      4           0        1        12
NetNS Name                    :        1     18          28        1        28
NetNS Context                 :        2 variable        88        2        88
Nexthop                       :        4    112         496        9      1116
Temporary memory              :        4 variable        48       11       356
Link Node                     :       78     12         936      147      1764
Link List                     :       41     24        1148       46      1288
Connected                     :        3     24          84        3        84
Interface                     :        4    188         752        4       752
Hash Index                    :       50 variable     14552       51     14596
Hash Bucket                   :      868     16       24304      870     24360
Hash                          :      100 variable      3552      100      3552
Graph Node                    :     4788     16      134064     4792    134176
Graph                         :       25      4         300       26       312
POSIX sync primitives         :        4 variable       176        4       176
FRR POSIX Thread              :        4 variable       240        4       240
RCU thread                    :        2     92         184        2       184
Access Filter                 :        2     88         184        2       184
Access List Str               :        1      4          12        1        12
Access List                   :        1     32          44        1        44
Lexer token (temporary)       :        0 variable         0        2        56
Command Argument Name         :     1057 variable     13228     1060     13264
Command Argument              :        2 variable        24        5        92
Command Token Help            :     2950 variable     39512     2952     39552
Command Token Text            :     2950 variable     87512     2952     87584
Command Tokens                :     4103     48      246180     4106    246360
Completion item               :        0 variable         0        4        48
Host config                   :        4 variable       144        4       144
Buffer data                   :        2   4108        8216        2      8216
Buffer                        :        7     12          84        7        84
Pseudowire                    :        1    176         188        1       188
--- qmem zebra ---
Type                          : Current#   Size       Total     Max#  MaxBytes
MH global info                :        1     40          44        1        44
ZEBRA VRF                     :        1   2656        2668        1      2668
RIB table info                :        4     16         112        4       112
Zebra Name Space              :        5 variable       492        5       492
Nexthop Group Connected       :        4     20         112        5       140
Nexthop Group Entry           :        4     56         240        6       360
RIB destination               :       13     44         572       13       572
Route Entry                   :        9     68         684       10       760
Zebra DPlane Provider         :        1    168         172        1       172
Zebra DPlane Ctx              :        0    496           0        6      3048
Router Advertisement Prefix   :        1     48          60        1        60
Zebra Netlink buffers         :        1 131072      131084        1    131084
Zebra Interface Information   :        4    328        1328        4      1328
--- qmem Table Manager ---
Type                          : Current#   Size       Total     Max#  MaxBytes
--- qmem Label Manager ---
Type                          : Current#   Size       Total     Max#  MaxBytes
---------------------------------------------------------------------------------------
Memory statistics for watchfrr:
System allocator statistics:
  Total heap allocated:  524 KiB
  Holding block headers: 0 bytes
  Used small blocks:     0 bytes
  Used ordinary blocks:  428 KiB
  Free small blocks:     784 bytes
  Free ordinary blocks:  96 KiB
  Ordinary blocks:       2
  Small blocks:          41
  Holding blocks:        0
(see system documentation for 'mallinfo' for meaning)
--- qmem logging subsystem ---
Type                          : Current#   Size       Total     Max#  MaxBytes
syslog target                 :        1     28          28        1        28
--- qmem libfrr ---
Type                          : Current#   Size       Total     Max#  MaxBytes
log thread-local buffer       :        1   5644        5644        1      5644
VTY                           :        4 variable     60400        4     60400
Vector index                  :     1149 variable     19052     1152     19180
Vector                        :     1149     12       13788     1152     13824
Typed-heap array              :        1    288         300        1       300
Thread stats                  :        9     36         396        9       396
Thread Poll Info              :        2   4096        8216        2      8216
Thread master                 :        3 variable     16612        3     16612
Thread                        :        5     88         460        5       460
Northbound Configuration      :        2      8          24        2        24
Temporary memory              :        4 variable        48        6       168
Link Node                     :       16     12         192       53       636
Link List                     :        6     24         168       11       308
Hash Index                    :       10 variable      3064       11      3108
Hash Bucket                   :      180     16        5040      180      5040
Hash                          :       20 variable       720       20       720
Graph Node                    :      565     16       15820      567     15876
Graph                         :        7      4          84        8        96
Lexer token (temporary)       :        0 variable         0        2        56
Command Argument Name         :       65 variable       940       68       976
Command Argument              :        2 variable        24        5        92
Command Token Help            :      360 variable      4912      362      4952
Command Token Text            :      360 variable     10448      362     10520
Command Tokens                :      453     48       27180      456     27360
Host config                   :        2 variable       104        2       104
Buffer data                   :        1   4108        4108        1      4108
Buffer                        :        4     12          48        4        48
--- qmem watchfrr ---
Type                          : Current#   Size       Total     Max#  MaxBytes
watchfrr daemon entry         :        1     76          76        1        76
```

