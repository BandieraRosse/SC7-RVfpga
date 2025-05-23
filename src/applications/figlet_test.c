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
void figlet_test()
{
    PRINT_COLOR(RED_COLOR_PRINT,"------------------------------figlet_test start-------------------------------------------\n");
    sc7_create_process(&process_figlet_entry);
}

void process_figlet_entry()
{
  PRINT_COLOR(process_figlet_color,"[figlet]process %d init!\n",myproc()->pid);
  char c[2]={'A','\0'};char c1[2]={'a','\0'};
  int ch;
  int switches_value;
  while(1)
  {
    PRINT_COLOR(process_figlet_color,"[figlet]process %d here\n",myproc()->pid);
    printf_figlet("process figlet here!");
    delay(1000);
    switches_value = READ_GPIO(GPIO_SWs) >> 16;
    ch = switches_value >> 14;
    if(ch == 1){
      PRINT_COLOR(RED_COLOR_PRINT,"------------------------------figlet_test end-------------------------------------------\n");
      wakeup(lk);
      exit();
    }else{
      self_sched();
    }

  }
}
