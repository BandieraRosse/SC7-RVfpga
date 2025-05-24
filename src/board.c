#include "types.h"

#define SegEn_ADDR 0x80001038
#define SegDig_ADDR 0x8000103C

#define GPIO_SWs 0x80001400
#define GPIO_LEDs 0x80001404
#define GPIO_INOUT 0x80001408
#define READ_GPIO(dir) (*(volatile unsigned *)dir)
#define WRITE_GPIO(dir, value)                 \
    {                                          \
        (*(volatile unsigned *)dir) = (value); \
    } //< 向地址写入四字节

/*
    下面给出操作switch和led，segdig的示例
*/

uint16 read_switches()
{
    int switches_value = READ_GPIO(GPIO_SWs);
    switches_value = switches_value >> 16;
    return (uint16)switches_value;
}

void enable_gpio_inout()
{
    WRITE_GPIO(GPIO_INOUT, 0xffff);
}

void set_leds(uint16 led_bit) //<设置LED灯
{
    WRITE_GPIO(GPIO_LEDs, led_bit);
}

void display_hex_number_segdig(uint64 hex_number) //< 根据给定8字节数据，显示8个16进制数
{
    WRITE_GPIO(SegDig_ADDR, hex_number);
}

void enable_segdig(uint16 enable_bit)
{
    WRITE_GPIO(SegEn_ADDR, enable_bit);
}