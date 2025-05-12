//complete all data type macro is in type.h

#include "types.h"
#include "test.h"
#include "print.h"
#include "process.h"
#include "string.h"
#include "cpu.h"

#include "bsp_printf.h"
#include "psp_timers_eh1.h"
/*也许会用到?*/
// #if defined(D_NEXYS_A7)
// #include <bsp_printf.h>
//    #include <bsp_mem_map.h>
//    #include <bsp_version.h>
// #else
//    PRE_COMPILED_MSG("no platform was defined")
// #endif
// #include <psp_api.h>

void init_process();
void create_process();
int SC7_start_kernel();
int printUartPutchar(char ch);
/*无论startup.S中call entry还是call main，都到达SC7_start_kernel*/
void entry()
{
  SC7_start_kernel();
  printf("never reach here\n");
  while(1);
}
int main()
{
  SC7_start_kernel();
  printf("never reach here\n");
  while(1);
} 

int SC7_start_kernel()
{
		uartInit();
    PRINT_COLOR(RED_COLOR_PRINT,"-----------------------------\n");
    PRINT_COLOR(GREEN_COLOR_PRINT,"SC7 is booting!\n");
    PRINT_COLOR(BLUE_COLOR_PRINT,"SC7_start_kernel at :%p\n",&SC7_start_kernel);
		for(int i=65;i<65+26;i++)
		{
			printUartPutchar(i);
			printUartPutchar('t');
		}
		printUartPutchar('\n');

		proc_init();
		PRINT_COLOR(YELLOW_COLOR_PRINT,"proc初始化完成\n");

    // test_print();
    // test_assert();
    
    //初始化init线程
    init_process();
    create_process();
    PRINT_COLOR(RED_COLOR_PRINT,"-----------------------------\n");
    PRINT_COLOR(RED_COLOR_PRINT,"进入scheduler\n");
    scheduler();
    //never reach here
	return 0;
}

#define WAIT_TIME 50000000 //D_CLOCK_RATE
// #define WAIT_TIME 32*1000*1000 //大约一秒
/*线程主动切换*/
void self_sched()
{
  struct proc* p = myproc();
  p->state = RUNNABLE;
  sched();
}

#define process_init_color GREEN_COLOR_PRINT
void process_init_entry()
{
  PRINT_COLOR(process_init_color,"process init here!\n");
  self_sched();
  PRINT_COLOR(process_init_color,"process init scheduled back!\n");
  while(1)
  {
    PRINT_COLOR(process_init_color,"process init here\n");
    uint64 i =pspTimerCounterGet(E_MACHINE_TIMER); //获取timer的value,已运行周期数
    PRINT_COLOR(process_init_color,"time is: %ld\n",i); //< 在RV32的机器上使用uint64也是可以的
    for(int i=0;i<WAIT_TIME;i++) //< 对应汇编是两句指令，所以time增量是WAIT_TIME的两倍
    {

    }
    self_sched();
  }
}

#define process1_color YELLOW_COLOR_PRINT
void process1_entry()
{
  PRINT_COLOR(process1_color,"process1 here!\n");
  self_sched();
  while(1)
  {
    PRINT_COLOR(process1_color,"process1 here\n");
    uint64 i =pspTimerCounterGet(E_MACHINE_TIMER); //获取timer的value,已运行周期数
    PRINT_COLOR(process1_color,"time is: %ld\n",i);
    for(int i=0;i<WAIT_TIME;i++)
    {
      
    }
    self_sched();
  }
}

/**
 * @brief 初始化init线程，进入scheduler后调度执行
 */
void init_process()
{
    struct proc* p = allocproc();
    p->state=RUNNABLE;
    p->context.ra = (uint64)process_init_entry;
}

void create_process()
{
    struct proc* p = allocproc();
    p->state=RUNNABLE;
    p->context.ra = (uint64)process1_entry;
}