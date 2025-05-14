#include "process.h"
#include "string.h"
#include "print.h"
#include "cpu.h"

#include "bsp_printf.h" //< 使用pspTimerCounterGet函数要包含这个，否则没有uint32_t的定义
#include "psp_timers_eh1.h"
extern void swtch(struct context *idle, struct context *p); //< swtch.S

#define PAGE_SIZE (0x1000)

struct proc pool[NPROC];
char kstack[NPROC][PAGE_SIZE];

/*下面三项事实上在NEXYS A7用不到*/
__attribute__((aligned(4096))) char ustack[NPROC][PAGE_SIZE]; //< 不支持用户态，不需要ustack
__attribute__((aligned(4096))) char trapframe[NPROC][PAGE_SIZE]; //< 不支持特权指令和特权级，不需要trapframe
__attribute__((aligned(4096))) char entry_stack[PAGE_SIZE]; //< 初始化在startup.S，不需要我们设置初始化的栈
//extern char boot_stack_top[];

int threadid()
{
	return myproc()->pid;
}

/*使用myproc()获取当前线程就行*/
// struct proc *curr_proc()
// {
// 	return myproc();
// }

// initialize the proc table at boot time.
void proc_init(void)
{
	struct proc *p;
	for (p = pool; p < &pool[NPROC]; p++) {
		p->state = UNUSED;
		p->kstack = (uint64)kstack[p - pool];
		p->ustack = (uint64)ustack[p - pool];
		p->trapframe = (struct trapframe *)trapframe[p - pool];
	}
}

int 
allocpid(void) 
{
    static int PID = 1;
    int pid = PID++;
    return pid;
}

// Look in the process table for an UNUSED proc.
// If found, initialize state required to run in the kernel.
// If there are no free procs, or a memory allocation fails, return 0.
struct proc *allocproc(void)
{
	struct proc *p;
	for (p = pool; p < &pool[NPROC]; p++) {
		if (p->state == UNUSED) {
			goto found;
		} 
	}
	return 0;

found:
	p->pid = allocpid();
	p->state = USED;
	memset(&p->context, 0, sizeof(p->context));
	memset(p->trapframe, 0, PAGE_SIZE);
	memset((void *)p->kstack, 0, PAGE_SIZE);
	//p->context.ra = (uint64)hsai_usertrapret; //< 设置为入口函数地址
	p->context.sp = p->kstack + PAGE_SIZE;
	return p;
}


void scheduler(void)
{
	struct proc *p;
	cpu_t* cpu = mycpu();
	cpu->proc = NULL;
	for (;;) {
		int i=0;
		for (p = pool; p < &pool[NPROC]; p++) {
			if (p->state == RUNNABLE) {
				//PRINT_COLOR(BLUE_COLOR_PRINT,"switch to process %d\n",i);
				p->state = RUNNING;
				cpu->proc = p;
				swtch(&mycpu()->context,&p->context);
				
				/* 返回这里时没有用户进程在CPU上执行 */
				cpu->proc = NULL;
			}
			i++; i%16;
			//printf("process %d is not RUNNABLE, skip.\n",i++);
		}
	}
}

void 
sched(void) 
{
  struct proc *p = myproc();
  if(p->state == RUNNING)
    panic("sched running");

  swtch(&p->context, &mycpu()->context);
}

void 
wakeup(void *chan) 
{
	struct proc *p;
	for (p = pool; p < &pool[NPROC]; p++) 
	{
	  if (p->state == SLEEPING && p->chan == chan) 
	  {
		p->state = RUNNABLE;
	  }
	}
  }