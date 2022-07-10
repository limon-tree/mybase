/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2019-05-23     tyustli   first version
 */

#include <rtthread.h>

int main(void)
{
    int count = 1;
    while (count++)
    {
        rt_thread_mdelay(500);
        rt_kprintf("hello rtthread\r\n");
    }

    return RT_EOK;
}
using namespace rtthread;

class tran
{
public:
    void getnumber(int a, int b)
    {
        x = a;
        y = b;
    }
    void out(tran & s)
    {
        rt_kprintf("x = %d, y = %d\n", x, y);
    }
private:
    int x, y;
};

int test_cpp(void)
{
    tran s;

    s.getnumber(13, 54);
    s.out(s);

    return 0;
}

MSH_CMD_EXPORT(test_cpp, test cpp);

#include<math.h>

#define MIN_VALUE                 (1e-4)
#define IS_DOUBLE_ZERO(d)         (abs(d) < MIN_VALUE)

double div_func(double x, double y)
{
    if (IS_DOUBLE_ZERO(y))
    {
        throw y;                                           /* throw exception */
    }

    return x / y;
}

void throw_exceptions(void *args)
{
    try
    {
        div_func(6, 3);
        rt_kprintf("there is no err\n");
        div_func(4, 0);                                   /* create exception*/
        rt_kprintf("you can run here\n");
    }
    catch(double)                                         /* catch exception */
    {
        rt_kprintf("error of dividing zero\n");
    }
}

MSH_CMD_EXPORT(throw_exceptions, throw cpp exceptions);
