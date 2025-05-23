#ifndef __PROCESS_H__
#define __PROCESS_H__

#include "types.h"

#define NPROC (16)

enum procstate { UNUSED, USED, SLEEPING, RUNNABLE, RUNNING, ZOMBIE };


#if defined RISCV
typedef struct context { //riscv 14个 //32位了
	uint32 ra;
	uint32 sp;

	// callee-saved
	uint32 s0;
	uint32 s1;
	uint32 s2;
	uint32 s3;
	uint32 s4;
	uint32 s5;
	uint32 s6;
	uint32 s7;
	uint32 s8;
	uint32 s9;
	uint32 s10;
	uint32 s11;
} context_t;
#else
typedef struct context //loongarch 12个
{
  uint64 ra;
  uint64 sp;

  // callee-saved
  uint64 s0;
  uint64 s1;
  uint64 s2;
  uint64 s3;
  uint64 s4;
  uint64 s5;
  uint64 s6;
  uint64 s7;
  uint64 s8;
  uint64 fp;
} context_t;
#endif

// Per-process state
typedef struct proc {
	enum procstate state; // Process state
	int pid; // Process ID
	uint64 ustack; // Virtual address of user stack 现在是物理地址
	uint64 kstack; // Virtual address of kernel stack
	struct trapframe *trapframe; // data page for trampoline.S
	struct context context; // swtch() here to run process

	void *chan;				///< 如果 non-zero，sleeping on chan
} proc_t;



void proc_init();
void scheduler() __attribute__((noreturn));
struct proc *allocproc();
void sched(void); 
void wakeup(void*);
void self_sched();
void exit();
void deleteproc(int pid);
void delay(uint64 time);
#endif // PROC_H