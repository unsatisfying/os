#include "types.h"
#include "screen.h"
#include "string.h"
#include "debug.h"
#include "gdt.h"
#include "idt.h"
#include "timer.h"
int kern_entry()
{
	init_debug();
	init_gdt();
	init_idt();
	screen_clear();

	printk_color(rc_black, rc_green, "HAPPY NEW YEAR!\n");
	
	init_timer(1000000);
	asm volatile ("int $0x3");
	asm volatile ("int $0x5");

	asm volatile("sti");
	
	//asm volatile("cli");
	return 0;
}
