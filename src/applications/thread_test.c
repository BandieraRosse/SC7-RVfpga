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


int tid1,tid2;
extern char *lk;

/* 线程 1 的入口函数 */
static void thread1_entry()
{
    uint64 count = 0;

    while (1)
    {
        /* 线程 1 采用低优先级运行，一直打印计数值 */
        printf("thread1 count: %d\n", count ++);
        delay(1000);
    }
}

/* 线程 2 入口 */
static void thread2_entry()
{
    uint64 count = 0;

    /* 线程 2 拥有较高的优先级，以抢占线程 1 而获得执行 */
    for (count = 0; count < 10 ; count++)
    {
        /* 线程 2 打印计数值 */
        printf("thread2 count: %d\n", count);
    }
    printf("thread2 exit\n");
}

void exitApp1(){
    while(1){
        if(READ_SW()>>14==1){
            printf("准备退出thread_sample\n");
            delete(tid1);
            delete(tid2);
            printf("成功退出thread_sample\n");
            delay(1000);
            wakeup(lk);
            exit();
            //rt_thread_delay(1000);
        }
        //rt_thread_delay(100);
    }
}
/* 线程示例 */
int thread_test(void)
{
    tid1 = sc7_create_process(&thread1_entry);
    tid2 = sc7_create_process(&thread2_entry);
    sc7_create_process(&exitApp1);

    return 0;
}

/* 导出到 msh 命令列表中 */