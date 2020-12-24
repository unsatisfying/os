#include "types.h"
#include "screen.h"
#include "string.h"
int kern_entry()
{
	screen_clear();
	screen_write_color("zrf.kernel,hello,kernel world!\n",rc_black,rc_cyan);
	screen_write_color("belibeli123456",rc_black,rc_cyan);
	return 0;
}
