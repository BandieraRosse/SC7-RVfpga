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
#include "pmem.h" // 包含物理内存管理头文件

extern char *lk;
/* 线程入口 */
void memtest_entry(void *parameter)
{
    int i;
    char *ptr = NULL;

    for (i = 0;; i++)
    {
        size_t request_size = 1 << i;
        ptr = kmalloc(request_size); // 分配一页

        if (ptr != NULL)
        {
            printf("Allocated %d bytes ,pid : %d\n", request_size, myproc()->pid);
            kfree(ptr); // 释放内存
            printf("Freed %d bytes\n", request_size);
            ptr = NULL;
        }
        else
        {
            panic("Failed to allocate %d bytes\n", request_size);
            return;
        }
        delay(500);
        yield();
    }
    exit();
}
int tid4;
void exitApp4()
{
    while (1)
    {
        if (READ_SW() >> 14 == 1)
        {
            deleteproc(tid4);
            PRINT_COLOR(RED_COLOR_PRINT, "------------------------------mem_test end-------------------------------------------\n");
            wakeup(lk);
            exit();
        }
        yield();
    }
}

int mem_test(void)
{
    PRINT_COLOR(RED_COLOR_PRINT, "------------------------------mem_test start-------------------------------------------\n");
    tid4 = sc7_create_process(&memtest_entry, 14);
    if (tid4 != NULL)
        sc7_start_process(tid4);
    int tid = sc7_create_process(&exitApp4, 14);
    if (tid != NULL)
        sc7_start_process(tid);

    return 0;
}
