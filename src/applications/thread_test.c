#include "types.h"
#include "test.h"
#include "print.h"
#include "process.h"
#include "string.h"
#include "cpu.h"
#include "figlet.h"
#include "board.h"

#include "bsp_printf.h"
#include "psp_timers_eh1.h"
#include "psp_api.h"

int tid01, tid02;
extern char *lk;

/* 线程 1 的入口函数 */
static void thread1_entry()
{
    uint64 count = 0;

    while (1)
    {
        /* 线程 1 采用低优先级运行，一直打印计数值 */
        printf("thread1 count: %d\n", count++);
        delay(1000);
        yield();
    }
}

/* 线程 2 入口 */
static void thread2_entry()
{
    uint64 count = 0;

    /* 线程 2 拥有较高的优先级，以抢占线程 1 而获得执行 */
    for (count = 0; count < 10; count++)
    {
        /* 线程 2 打印计数值 */
        printf("thread2 count: %d\n", count);
        delay(200);
        yield();
    }
    printf("thread2 exit\n");
    exit();
}

void exitApp0()
{
    while (1)
    {
        if (READ_SW() >> 14 == 1)
        {
            deleteproc(tid01);
            deleteproc(tid02);
            PRINT_COLOR(RED_COLOR_PRINT, "------------------------------thread_test end -------------------------------------------\n");
            delay(1000);
            wakeup(lk);
            exit();
        }
        yield();
    }
}
/* 线程示例 */
int thread_test(void)
{
    PRINT_COLOR(RED_COLOR_PRINT, "------------------------------thread_test start-------------------------------------------\n");
    tid01 = sc7_create_process(&thread1_entry, 14);
    if (tid01 != NULL)
        sc7_start_process(tid01);
    yield();
    delay(3000);   
    tid02 = sc7_create_process(&thread2_entry, 13);
    if (tid02 != NULL)
        sc7_start_process(tid02);
    int tid = sc7_create_process(&exitApp0, 14);
    if (tid != NULL)
        sc7_start_process(tid);

    return 0;
}

/* 导出到 msh 命令列表中 */