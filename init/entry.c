#include "types.h"
#include "screen.h"
#include "string.h"
#include "debug.h"
#include "gdt.h"
#include "idt.h"
#include "timer.h"
#include "pmm.h"
int kern_entry()
{
	init_debug();
	init_gdt();
	init_idt();
	screen_clear();

	printk_color(rc_black, rc_green, "HAPPY NEW YEAR!\n");

	init_timer(200);

	//asm volatile("sti");

	printk("Kernel start addr in memory: 0x%08X\n", kern_start);
	printk("Kernel end addr in memory: 0x%08X\n", kern_end);
	printk("Kernel used %d KB in memory!\n\n\n", (kern_end - kern_start + 1) / 1024);

	show_memory_map();
	//asm volatile("cli");
	return 0;
}
