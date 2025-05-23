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
#include "pmem.h"  // 包含物理内存管理头文件


extern char *lk;
/* 线程入口 */
void memtest_entry(void *parameter)
{
    int i;
    char *ptr = NULL;

    for (i = 0; ; i++)
    {
        size_t request_size = 1 << i;
        size_t pages_needed = (request_size + PGSIZE - 1) / PGSIZE;

        ptr = kalloc(); // 分配一页

        if (ptr != NULL)
        {
            printf("Allocated %d bytes ,pid : %d\n", request_size,myproc()->pid);
            //kfree(ptr); // 释放内存
            printf("Freed %d bytes\n", request_size);
            ptr = NULL;
        }
        else
        {
            printf("Failed to allocate %d bytes\n", request_size);
            return;
        }
        delay(1000);
        self_sched();
    }
    exit();
}
int tid = 0;
void exitApp3(){
    while(1){
        if(READ_SW()>>14==1){
            printf("准备退出mem_test\n");
            printf("delete tid %d\n",tid);
            deleteproc(tid);
            printf("成功退出mem_test\n");
            wakeup(lk);
            exit();
        }
        self_sched();
    }
}


int mem_test(void)
{
    tid = sc7_create_process(&memtest_entry);
    printf("mem_test tid : %d\n",tid);
    sc7_create_process(&exitApp3);

    return 0;
}
