#include "common.h"
#include "string.h"
#include "debug.h"
#include "idt.h"

idt_entry_t  idt_entries[256];
idt_ptr_t idt_ptr;

interrupt_handler_t interrupt_handlers[256];

// 设置中断描述符
static void idt_set_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags);

// 声明加载 IDTR 的函数
extern void idt_flush(uint32_t);

void init_idt()
{
	//两片Intel 8259A 芯片
	//主片端口 0x20  0x21
	//从片端口 0xA0  0xA1

	//初始化主从芯片的ICW1
	// 0x11 即0001 0001
	//0		0		0		1		0		0		0		1
	//A7    A6      A5      1 （恒为1）     LTIM    ADI     SNGL    IC4
	//A7-A5表示在MCS80/85中用于中断服务过程的页面起始地址，与ICW2中的A15-A8共同组成
	//LTIM  1-电平触发中断  当8259A检测到 IRi（i＝0～7）端有高电平时产生中断。在这种触发方式中，要求触发电平必须保持到中断响应信号INTA有效为止。在CPU响应中断后，应及时撤销该请求信号，以防止CPU再次响应，出现重复中断现象。
	//LTIM  0-边缘触发中断	当 LTIM=0 时，为边沿触发方式。当8259A检测到IRi端有由低到高的跳变（上升沿）信号时产生中断。
	//SNGL  1-单片   0-多片
	//IC4  1-需要ICW4   0-不需要
	outb(0x20,0x11);
	outb(0xA0,0x11);
	//然后是初始化ICW2，它的高五位表示送出的中断号的高五位，低三位由芯片根据当前的IR0-IR7自动填入对应的二进制串
	//通过前面的设计知道0-31号中断是cpu保留的，所以这里主片的IR0就从第32号中断开始，从片的IR0就是第40号中断。
	//如何设置ICW2呢，在ICW1设置之后，地址线A0会变成1，这时代表设置的就是ICW2，当A0为1时，主片的端口是0x21，从片的端口是0xA1
	//主片终端号从32-39		0x20=32b
	outb(0x21,0x20);
	//从片中断号从40-47 0x28=40b
	outb(0xA1,0x28);

	//主片的ICW3的8位代表的是连接从片的情况，哪一位为1就代表哪一位的IRx连接了从片，这里是IR2连接从片所以值为0x04(0000 0100)
	outb(0x21,0x04);
	//从片的ICW3高五位为0，低三位表示的是该从片连接的是主片的哪一个IR，
	//所以这里设置成0x02代表该从片连接的是主片的IR2
	//设置从片连接主片的IR2
	outb(0xA1,0x02);

	//这里设置成0x01。表示 8259A 芯片被设置成普通全嵌套、非缓冲、非自动结束中断方式，并且用于 8086 及其兼容系统。
	//设置芯片以8086方式工作
	outb(0x21,0x01);
	outb(0xA1,0x01);

	//现在设置一下OCW1，OCW1的八位，哪一位为1代表屏蔽哪一级中断，现在我们都不屏蔽
	//开放所有中断
	outb(0x21,0x00); 
	outb(0xA1,0x00);

    memset((uint8_t *)&interrupt_handlers, 0, sizeof(interrupt_handler_t) * 256);

    idt_ptr.limit=sizeof(idt_entry_t)*256-1;
    idt_ptr.base=(uint32_t) &idt_entries;

    memset((uint8_t*) &idt_entries,0,sizeof(idt_entry_t)*256);

	// 0-32:  用于 CPU 的中断处理       目标代码段为gdt表中下标为1的位置，所以是0x08,一个描述符8字节。
    //                                段存在标志p存在， DPL特权级别为0,剩下五位01110表示中断门，01111表示陷阱门，00101表示任务门，这里是中断门。
	idt_set_gate( 0, (uint32_t)isr0,  0x08, 0x8E);
	idt_set_gate( 1, (uint32_t)isr1,  0x08, 0x8E);
	idt_set_gate( 2, (uint32_t)isr2,  0x08, 0x8E);
	idt_set_gate( 3, (uint32_t)isr3,  0x08, 0x8E);
	idt_set_gate( 4, (uint32_t)isr4,  0x08, 0x8E);
	idt_set_gate( 5, (uint32_t)isr5,  0x08, 0x8E);
	idt_set_gate( 6, (uint32_t)isr6,  0x08, 0x8E);
	idt_set_gate( 7, (uint32_t)isr7,  0x08, 0x8E);
	idt_set_gate( 8, (uint32_t)isr8,  0x08, 0x8E);
	idt_set_gate( 9, (uint32_t)isr9,  0x08, 0x8E);
	idt_set_gate(10, (uint32_t)isr10, 0x08, 0x8E);
	idt_set_gate(11, (uint32_t)isr11, 0x08, 0x8E);
	idt_set_gate(12, (uint32_t)isr12, 0x08, 0x8E);
	idt_set_gate(13, (uint32_t)isr13, 0x08, 0x8E);
	idt_set_gate(14, (uint32_t)isr14, 0x08, 0x8E);
	idt_set_gate(15, (uint32_t)isr15, 0x08, 0x8E);
	idt_set_gate(16, (uint32_t)isr16, 0x08, 0x8E);
	idt_set_gate(17, (uint32_t)isr17, 0x08, 0x8E);
	idt_set_gate(18, (uint32_t)isr18, 0x08, 0x8E);
	idt_set_gate(19, (uint32_t)isr19, 0x08, 0x8E);
	idt_set_gate(20, (uint32_t)isr20, 0x08, 0x8E);
	idt_set_gate(21, (uint32_t)isr21, 0x08, 0x8E);
	idt_set_gate(22, (uint32_t)isr22, 0x08, 0x8E);
	idt_set_gate(23, (uint32_t)isr23, 0x08, 0x8E);
	idt_set_gate(24, (uint32_t)isr24, 0x08, 0x8E);
	idt_set_gate(25, (uint32_t)isr25, 0x08, 0x8E);
	idt_set_gate(26, (uint32_t)isr26, 0x08, 0x8E);
	idt_set_gate(27, (uint32_t)isr27, 0x08, 0x8E);
	idt_set_gate(28, (uint32_t)isr28, 0x08, 0x8E);
	idt_set_gate(29, (uint32_t)isr29, 0x08, 0x8E);
	idt_set_gate(30, (uint32_t)isr30, 0x08, 0x8E);
	idt_set_gate(31, (uint32_t)isr31, 0x08, 0x8E);

	//32-47  8259A中断
	idt_set_gate(32, (uint32_t)irq0, 0x08, 0x8E);
    idt_set_gate(33, (uint32_t)irq1, 0x08, 0x8E);
    idt_set_gate(34, (uint32_t)irq2, 0x08, 0x8E);
    idt_set_gate(35, (uint32_t)irq3, 0x08, 0x8E);
    idt_set_gate(36, (uint32_t)irq4, 0x08, 0x8E);
    idt_set_gate(37, (uint32_t)irq5, 0x08, 0x8E);
    idt_set_gate(38, (uint32_t)irq6, 0x08, 0x8E);
    idt_set_gate(39, (uint32_t)irq7, 0x08, 0x8E);
    idt_set_gate(40, (uint32_t)irq8, 0x08, 0x8E);
    idt_set_gate(41, (uint32_t)irq9, 0x08, 0x8E);
    idt_set_gate(42, (uint32_t)irq10, 0x08, 0x8E);
    idt_set_gate(43, (uint32_t)irq11, 0x08, 0x8E);
    idt_set_gate(44, (uint32_t)irq12, 0x08, 0x8E);
    idt_set_gate(45, (uint32_t)irq13, 0x08, 0x8E);
    idt_set_gate(46, (uint32_t)irq14, 0x08, 0x8E);
    idt_set_gate(47, (uint32_t)irq15, 0x08, 0x8E);

	// 255 将来用于实现系统调用
	idt_set_gate(255, (uint32_t)isr255, 0x08, 0x8E);

	// 更新设置中断描述符表
	idt_flush((uint32_t)&idt_ptr);
}

static void idt_set_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags)
{
    idt_entries[num].base_lo=base&0xFFFF;
    idt_entries[num].base_hi=(base>>16)&0xFFFF;
    idt_entries[num].always0=0;
    idt_entries[num].sel=sel;
    idt_entries[num].flags=flags;//|0x60 用以设置特权级别为3
}

//注册中断处理函数
void register_interrupt_handler(uint8_t n, interrupt_handler_t h)
{
    interrupt_handlers[n]=h;
}

//调用中断处理函数
void isr_handler(pt_regs *regs)
{
    if(interrupt_handlers[regs->int_number])
    {
        interrupt_handlers[regs->int_number](regs);
    }
    else
    {
        printk_color(rc_black,rc_blue, "Unhandled interrupt: %d\n", regs->int_number);
    }
    
}
void irq_handler(pt_regs *regs)
{
	//如果中断号小于40表明由主片控制，给主片发送重置信号，即由OCW2寄存器控制
	//0x20 即0010 0000
	//D7：R 0-非循环优先级方式  1-循环优先级方式
	//D6：SL 0不设置EIO命令	1-设置EIO命令
	//D5：SL=0时候，L2-L0无效 SL=1时候，L2-L0有效	中断结束命令位。EOI＝1使当前ISR寄存器的相应位清0。当ICW4中的AEOI为0时，ISR中的相应置位就要由该命令位来清除。
	//D4-D3：0 0   OCW2标志
	//D2-D0：L2-L0   表示ISRi序号 二进制。0-7或者8-15
	if(regs->int_number<40)
		outb(0x20,0x20);
	else
	{
		outb(0xA0,0x20);
	}

	if(interrupt_handlers[regs->int_number])
	{
		interrupt_handlers[regs->int_number](regs);
	}
	else
    {
        printk_color(rc_black,rc_blue, "Unhandled interrupt: %d\n", regs->int_number);
    }
	
}

