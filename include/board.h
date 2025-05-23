#ifndef __BOARD_H__
#define __BOARD_H__


#define SegEn_ADDR      0x80001038
#define SegDig_ADDR     0x8000103C
//#define WAIT_TIME 50000000 //D_CLOCK_RATE
#define WAIT_TIME 32*1000*1000 //大约一秒



#define GPIO_SWs 0x80001400
#define GPIO_LEDs 0x80001404
#define GPIO_INOUT 0x80001408
#define READ_GPIO(dir) (*(volatile unsigned *)dir)
#define WRITE_GPIO(dir, value) { (*(volatile unsigned *)dir) = (value); } //< 向地址写入四字节
#define READ_SW() (READ_GPIO(GPIO_SWs) >> 16)

#endif


