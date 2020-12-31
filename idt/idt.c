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
    memset((uint8_t *)&interrupt_handlers, 0, sizeof(interrupt_handler_t) * 256);

    idt_ptr.limit=sizeof(idt_entry_t)*256-1;
    idt_ptr.base=(uint32_t) &idt_entries;

    memset((uint8_t*) &idt_entries,0,sizeof(idt_entry_t)*256);

	// 0-32:  用于 CPU 的中断处理       目标代码段为gdt表中下标为1的位置，所以是0x08 
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


