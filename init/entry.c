#include "types.h"
#include "screen.h"
#include "string.h"
#include "debug.h"
#include "gdt.h"
int kern_entry()
{
	init_debug();
	init_gdt();

	screen_clear();

	printk_color(rc_black, rc_green, "Hello, OS kernel!\n");

	return 0;
}
