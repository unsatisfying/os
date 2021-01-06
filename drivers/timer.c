//时钟8253/8254 Timer驱动

#include "timer.h"
#include "common.h"
#include "debug.h"
#include "idt.h"
//时钟中断的回调函数
void timer_callback(pt_regs *regs)
{
    static uint32_t tick = 0;
    printk_color(rc_black, rc_blue, "Tick: %d\n", tick++);
}
// timer_callback函数就产生中断是调用的中断处理函数，这里只是测试就写了打印
// 8253/8254芯片的工作模式3下是方波发生器方式，
//该方式下产生的方波的频率为输入时钟频率的N分之一，
//该芯片输入时钟是1193180HZ，
// N就是我们设置给它的参数，
//所以我们想它每10ms(100HZ)产生一次IRQ0的信号也就是每10ms产生一个方波,
//那么N取值就是1193180/100
void init_timer(uint32_t frequency)
{
    register_interrupt_handler(IRQ0, timer_callback);

    // Intel 8253/8254 PIT芯片 I/O端口地址范围是40h~43h
    // 输入频率为 1193180/frequency 即每秒中断次数
    uint32_t divisor = 1193180 / frequency;

    // D7 D6 D5 D4 D3 D2 D1 D0
    // 0  0  1  1  0  1  1  0
    // 即就是 36 H
    // 设置 8253/8254 芯片工作在模式 3 下
    outb(0x43, 0x36);

    // 拆分低字节和高字节
    uint8_t low = (uint8_t)(divisor & 0xFF);
    uint8_t hign = (uint8_t)((divisor >> 8) & 0xFF);

    // 分别写入低字节和高字节
    outb(0x40, low);
    outb(0x40, hign);
}