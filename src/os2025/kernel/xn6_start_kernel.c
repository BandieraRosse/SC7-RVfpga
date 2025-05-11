//complete all data type macro is in type.h

//this program only need these
//wait to be unified in style
#include "types.h"
#include "test.h"
#include "uart.h"
#include "print.h"
#include "process.h"
#include "pmem.h"
#include "string.h"
#include "virt.h"
#include "hsai_trap.h"
#include "plic.h"
#include "vmem.h"
#include "riscv_memlayout.h"
#if defined RISCV
  #include "riscv.h"
#else
  #include "loongarch.h"
#endif

extern struct proc *current_proc;
void uservec(){return ;}//不使用
void userret(uint64 trapframe){return ;}//不使用
extern int init_main( void ); //用户程序

__attribute__((aligned(4096))) char user_stack[4096] ;//用户栈

int main() { while ( 1 ); } //为了通过编译, never use this

void  test_pmem();
void virtio_writeAndRead_test();
void loongarch_user_program_run();
void riscv_user_program_run();
void user_program_run();
void init_process();
void create_process();
extern void kernelvec();

struct buf buf;//临时用来测试磁盘读写

#include "bsp_printf.h"
#include "psp_timers_eh1.h"
int xn6_start_kernel();
int printUartPutchar(char ch);
void entry()
{
  xn6_start_kernel();
  printf("never reach here\n");
  while(1);
}
int xn6_start_kernel()
{
	//if ( hsai::get_cpu()->get_cpu_id() == 0 )
		uartInit();
		for(int i=65;i<65+26;i++)
		{
			printUartPutchar(i);
			printUartPutchar('t');
		}
		printUartPutchar('\n');
    PRINT_COLOR(BLUE_COLOR_PRINT,"xn6_start_kernel at :%p\n",&xn6_start_kernel);

		proc_init();
		printf("proc初始化完成\n");
    
    //初始化init线程
    init_process();
    create_process();
    scheduler();
    // while(1) ;

    //never reach
	return 0;
}

void sched();
proc_t* myproc(void);
#define WAIT_TIME 1000*100*1024 //5秒
/*线程主动切换，都只在内核态*/
void self_sched()
{
  struct proc* p = myproc();
  p->state = RUNNABLE;
  sched();
}

void process_init_entry()
{
  printf("process init here!\n");
  self_sched();
  printf("process init scheduled back!\n");
  while(1)
  {
    printf("init process here\n");
    int i =pspTimerCounterGet(E_MACHINE_TIMER); //获取timer的value,已运行周期数
    printf("have been running for: %d\n",i);
    for(int i=0;i<WAIT_TIME;i++)
    {

    }
    self_sched();
  }
}

void process1_entry()
{
  printf("process1 here!\n");
  self_sched();
  while(1)
  {
    printf("process1 here\n");
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
    current_proc =p ;
    p->state=RUNNABLE;
    p->context.ra = (uint64)process_init_entry;
    // hsai_set_trapframe_epc(p->trapframe, (uint64)init_main);
    // hsai_set_trapframe_user_sp(p->trapframe,(uint64)user_stack+4096);
}

void create_process()
{
    struct proc* p = allocproc();
    p->state=RUNNABLE;
    p->context.ra = (uint64)process1_entry;
}