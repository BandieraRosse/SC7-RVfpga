//complete all data type macro is in type.h

#include "types.h"
#include "test.h"
#include "print.h"
#include "process.h"
#include "string.h"
#include "cpu.h"
#include "figlet.h"

#include "bsp_printf.h"
#include "psp_timers_eh1.h"

void process1_entry();
void process_init_entry();
void process_figlet_entry();
void sc7_create_process(void (*process_entry)(void));
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
    // PRINT_COLOR(GREEN_COLOR_PRINT,"SC7 is booting!\n");
    printf_figlet("SC7 is booting!"); //< 艺术字打印
    PRINT_COLOR(BLUE_COLOR_PRINT,"SC7_start_kernel at :%p\n",&SC7_start_kernel); //< 颜色打印
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
    
    //初始化线程
    sc7_creat_process(&process_init_entry);
    sc7_creat_process(&process1_entry);
    sc7_creat_process(&process_figlet_entry);
    sc7_creat_process(&process1_entry);
    sc7_creat_process(&process1_entry);

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
  PRINT_COLOR(process_init_color,"process %d init!\n",myproc()->pid);
  while(1)
  {
    PRINT_COLOR(process_init_color,"process %d here\n",myproc()->pid);
    uint64 i =pspTimerCounterGet(E_MACHINE_TIMER); //获取timer的value,已运行周期数
    PRINT_COLOR(process_init_color,"time is: %p\n",i); //< 显示timer的value
    for(int i=0;i<WAIT_TIME;i++) //< 对应汇编是两句指令，所以time增量是WAIT_TIME的两倍
    {

    }
    self_sched(); //< 主动让出
  }
}

#define process1_color YELLOW_COLOR_PRINT
void process1_entry()
{
  PRINT_COLOR(process1_color,"process %d init!\n",myproc()->pid);
  while(1)
  {
    PRINT_COLOR(process1_color,"process %d here\n",myproc()->pid);
    uint64 i =pspTimerCounterGet(E_MACHINE_TIMER); //获取timer的value,已运行周期数
    PRINT_COLOR(process1_color,"time is: %p\n",i);
    for(int i=0;i<WAIT_TIME;i++)
    {
      
    }
    self_sched();
  }
}

#define process_figlet_color COLOR_RESET
void process_figlet_entry()
{
  PRINT_COLOR(process_figlet_color,"process %d init!\n",myproc()->pid);
  char c[2]={'A','\0'};char c1[2]={'a','\0'};
  while(1)
  {
    PRINT_COLOR(process_figlet_color,"process %d here\n",myproc()->pid);
    printf_figlet("process figlet here!");
    for(int i=0;i<WAIT_TIME;i++)
    {
      
    }
    self_sched();
  }
}

void sc7_creat_process(void (*process_entry)(void)) //< 给定入口函数，创建process
{
    struct proc* p = allocproc();
    p->state=RUNNABLE;
    p->context.ra = (uint64)process_entry;
}