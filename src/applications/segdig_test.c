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

void process_figlet_entry();
void process_segdig_entry();
extern void yield();
extern void exit();

#define process_figlet_color COLOR_RESET

extern char *lk;
void process_segdig_entry() //<显示时间到数码管
{
    int En_Value = 0xFFFF;
    WRITE_GPIO(GPIO_INOUT, En_Value);
    while (1)
    {
        printf("[segdig]process %d here\n", myproc()->pid);
        // WRITE_GPIO(SegDig_ADDR, 0x00111309);
        uint64 i = pspTimerCounterGet(E_MACHINE_TIMER);
        WRITE_GPIO(SegDig_ADDR, i);
        printf("[segdig]time is %p\n", i);
        delay(1000);
        yield();
    }
}
int tid2;
void exitApp2()
{
    while (1)
    {
        if (READ_SW() >> 14 == 1)
        {
            deleteproc(tid2);
            PRINT_COLOR(RED_COLOR_PRINT, "------------------------------segdig_test end -------------------------------------------\n");
            printf("成功退出segdig_sample\n");
            delay(1000);
            wakeup(lk);
            exit();
        }
        yield();
    }
}

void segdig_test()
{
    PRINT_COLOR(RED_COLOR_PRINT, "------------------------------segdig_test start-------------------------------------------\n");
    tid2 = sc7_create_process(&process_segdig_entry, 14);
    if (tid2 != NULL)
        sc7_start_process(tid2);
    int tid = sc7_create_process(&exitApp2, 14);
    if (tid != NULL)
        sc7_start_process(tid);
}
