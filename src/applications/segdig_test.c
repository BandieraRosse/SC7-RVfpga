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
extern void self_sched();
extern void exit();

#define process_figlet_color COLOR_RESET

extern char *lk;
void segdig_test()
{
    PRINT_COLOR(RED_COLOR_PRINT,"------------------------------segdig_test start-------------------------------------------\n");
    sc7_create_process(&process_segdig_entry);
}

void process_segdig_entry() //<显示时间到数码管
{
  int En_Value=0xFFFF, switches_value;
  WRITE_GPIO(GPIO_INOUT,En_Value);
  while (1) 
  { 
    printf("[segdig]process %d here\n",myproc()->pid);
    
    //WRITE_GPIO(SegDig_ADDR, 0x00111309);
    uint64 i =pspTimerCounterGet(E_MACHINE_TIMER);
    WRITE_GPIO(SegDig_ADDR, i);
    printf("[segdig]time is %p\n",i);
    delay(1000);
    if(READ_SW()>>14==1){
      PRINT_COLOR(RED_COLOR_PRINT,"------------------------------segdig_test end-------------------------------------------\n");
      wakeup(lk);
      exit();
    }else{
      self_sched();
    }
  }
}