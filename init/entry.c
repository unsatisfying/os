#include "types.h"
#include "screen.h"
#include "string.h"
int kern_entry()
{
	init_debug();

	screen_clear();

	printk_color(rc_black, rc_green, "Hello, OS kernel!\n");

	panic("test");
	return 0;
}
