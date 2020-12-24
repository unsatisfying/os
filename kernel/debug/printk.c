#include "screen.h"
#include "string.h"
#include "debug.h"
#include "vargs.h"

static int vsprintf(char *buff,const char *format,va_list args);
void printk(const char *format,...)
{
    static char buff[1024];
    va_list args;
    va_start(args,format);
    int i=vsprintf(buff,format,args);
    va_end(args);
    if(i>1024)
        screen_write("printk overflow!\n");
    buff[i]='\0';
    screen_write(buff);
}

void printk_color(real_color_t back, real_color_t fore, const char *format, ...)
{
	// 避免频繁创建临时变量，内核的栈很宝贵
	static char buff[1024];
	va_list args;
	int i;

	va_start(args, format);
	i = vsprintf(buff, format, args);
	va_end(args);
    if(i>1024)
        screen_write("printk overflow!\n");

	buff[i] = '\0';

	screen_write_color(buff, back, fore);
}


#define is_digit(c)	((c) >= '0' && (c) <= '9') //判断是否是数字
#define do_div(n,base) ({ \
		int __res; \
		__asm__("divl %4":"=a" (n),"=d" (__res):"0" (n),"1" (0),"r" (base)); \
		__res; })// 32位相除，商在EAX，余数在EDX，返回__res
static int skip_atoi(const char **s)  //把字符表述的整型数字转化成真正的整型，因为这里要改变 *s,所以要传 **s.把当前指针移动到转化了数字之后的那个字符去，返回那个数字
{
    int i = 0;

    while (is_digit(**s)) {
        i = i * 10 + *((*s)++) - '0';
    }

    return i;
}

#define ZEROPAD		1	// pad with zero 0填充 例如"%04d" 表示4位宽度，不够前面补0；
#define SIGN	 	2   // unsigned/signed long  "%d"有符号十进制整数 "%i"有符号十进制整数(与%d相同)，表示有符号；； "%u"表示无符号%u   无符号十进制整数
#define PLUS    	4	// show plus ”+“ 输出正号
#define SPACE	  	8   // space if plus    如果值为正则输出空格。
#define LEFT	 	16  // left justified "-" 左对齐输出，右边填空格“%-20s”左对齐输出字符串，长度为20个字符
#define SPECIAL		32  // 0x ”#“ 如果为%o表示8进制，前缀加o  如果为”%x“表示16进制，前缀为0x
#define SMALL	  	64  // use 'abcdef' instead of 'ABCDEF'

static int vsprintf(char *buff,const char *format,va_list args)
{
    char *str=buff;
    for(;*format;++format)
    {
        if(*format!='%')
        {
            *str++=*format;
            continue;
        }
    }

    int flags=0;
}
