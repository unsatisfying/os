#include "types.h"
#include "screen.h"
#include "string.h"
int kern_entry()
{
	screen_clear();
	char c, s[50]; 
	int a=1234;
	strcpy(s, "Hello,World"); 
	c='\x41'; 
	printk("a=%d\n", a);//按照十进制整数格式输出，显示 a=1234
	printk("a=%d%%\n", a);//输出%号 结果 a=1234%
	printk("a=%6d\n", a);//输出6位十进制整数 左边补空格，显示 a= 1234
	printk("a=%06d\n", a);//输出6位十进制整数 左边补0，显示 a=001234
	//printk("a=%2d\n", a);//a超过2位，按实际输出 a=1234
	//printk("a=%-6d\n", a);///输出6位十进制整数 右边补空格，显示 a=1234
	printk("c=%c\n", c);     //输出字符 c=A
	printk("c=%x\n", c);//以十六进制输出字符的ASCII码 c=41
	printk("s[]=%s\n", s);//输出数组字符串s[]=Hello,World
	printk_color(rc_black,rc_cyan,"s[]=%6.9s\n", s);//输出最多9个字符的字符串 s[]=Hello,Wor
	screen_write_color("zrf.kernel,hello,kernel world!\n",rc_black,rc_cyan);
	printk("a=%d\n", a);//按照十进制整数格式输出，显示 a=1234
	printk("a=%d%%\n", a);//输出%号 结果 a=1234%
	printk("a=%6d\n", a);//输出6位十进制整数 左边补空格，显示 a= 1234
	printk("a=%06d\n", a);//输出6位十进制整数 左边补0，显示 a=001234
	//screen_write_color("zrf.kernel,hello,kernel world!\n",rc_black,rc_cyan);
	return 0;
}
