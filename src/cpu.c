#include "cpu.h"

cpu_t cpus[NCPU];

/**
 * @brief 初始化cpu
 */
void 
cpuinit(void) 
{
    struct cpu *it;
    for (it = cpus; it < &cpus[NCPU]; it++) 
    {
      it->proc = 0;
      it->intena = 0;
      it->noff = 0;
    }
}

/**
 * @brief 返回当前CPU
 */
cpu_t* 
mycpu(void) 
{
    int hartid = 0;
    return &cpus[hartid];
}

/**
 * @brief 返回当前CPU对应的process
 */
proc_t* 
myproc(void) 
{
    proc_t* p = mycpu()->proc;
    return p;
}
