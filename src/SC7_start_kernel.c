//complete all data type macro is in type.h

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

void process1_entry();
void process_init_entry();
void process_figlet_entry();
void process_led_entry();
void process_segdig_entry();
int sc7_create_process(void (*process_entry)(void));
int SC7_start_kernel();
int printUartPutchar(char ch);
void start_APP();
void exit();
/*无论startup.S中call entry还是call main，都到达SC7_start_kernel*/

char *lk;
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
    PRINT_COLOR(RED_COLOR_PRINT,"-------------------------------------------------------------------------\n");
    // PRINT_COLOR(GREEN_COLOR_PRINT,"SC7 is booting!\n");
    printf_figlet("SC7 Is Booting!"); //< 艺术字打印
    PRINT_COLOR(BLUE_COLOR_PRINT,"SC7_start_kernel at :%p\n",&SC7_start_kernel); //< 颜色打印
		for(int i=65;i<65+26;i++)
		{
			printUartPutchar(i);
			printUartPutchar('t');
		}
		printUartPutchar('\n');

    pmem_init();
		proc_init();
		PRINT_COLOR(YELLOW_COLOR_PRINT,"proc初始化完成\n");

    // test_print();
    // test_assert();
    
    //初始化线程
    sc7_create_process(&start_APP);
    // sc7_create_process(&process_init_entry);
    // sc7_create_process(&process1_entry);
    //sc7_create_process(&process_led_entry);
    //sc7_create_process(&process_segdig_entry);

    PRINT_COLOR(RED_COLOR_PRINT,"-------------------------------------------------------------------------\n");
    PRINT_COLOR(RED_COLOR_PRINT,"进入scheduler\n");
    scheduler();
    //never reach here
	return 0;
}

//#define WAIT_TIME 50000000 //D_CLOCK_RATE
// #define WAIT_TIME 32*1000*1000 //大约一秒
/*线程主动切换*/
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



void process_led_entry()
{
  int En_Value=0xFFFF, switches_value;
  WRITE_GPIO(GPIO_INOUT,En_Value);
  while (1) 
  { 
    printf("[led]process %d here\n",myproc()->pid);
    switches_value = READ_GPIO(GPIO_SWs);
    switches_value = switches_value >> 16;
    printf("[led]switches_value:");
    for(int i=15;i>=0;i--)//<逐位输出开关的值，从左到右，从sw15到sw0
    {
      int mask=1<<i;
      int led_bit=switches_value & mask;
      led_bit=led_bit>>i;
      printf("%d",led_bit);
    }
    printf("\n");//<输出完记得换行
    WRITE_GPIO(GPIO_LEDs, switches_value);

    for(int i=0;i<WAIT_TIME;i++)
    {
      
    }
    self_sched();
  }
}



int sc7_create_process(void (*process_entry)(void)) //< 给定入口函数，创建process
{
    struct proc* p = allocproc();
    p->state=RUNNABLE;
    p->context.ra = (uint64)process_entry;
    printf("create  process %d\n",p->pid);
    return p->pid;
}

void printInfo(){
  printf("--------------------------------------------------------------------------\n");
  printf("更改SW0-3 进入不同应用,SW15-1 为10表示确认,01表示退出\n");
  printf("0000:thread_sample()\n");
  printf("0001:figlet_test()\n");
  printf("0002:segdig_test()\n");
  printf("0004:mem_test()\n");
  printf("--------------------------------------------------------------------------\n");
}
void start_APP(){
   int ch;
   int switches_value;
   printInfo();
   while(1){
      switches_value = READ_GPIO(GPIO_SWs) >> 16;
      ch = switches_value >> 14;
      if(ch == 2){
        switch(switches_value)
        {
          case 0x8000:
            thread_test();
            sleep(lk);
            printInfo();
            break;
          case 0x8001:
            figlet_test();
            sleep(lk);
            printInfo();
            break;
          case 0x8002:
            segdig_test();
            sleep(lk);
            printInfo();
            break;
          case 0x8004:
            mem_test();
            sleep(lk);
            printInfo();
            break;
        }
      }
  } 
}