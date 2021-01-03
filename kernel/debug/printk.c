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
    {
        screen_write("printk overflow!\n");
    }
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
    {
        screen_write("printk overflow!\n");
    }
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


static char *number(char *str,int num,int base/*进制数*/,int size/*字符长度，如%20d，规定长度为20*/,int precision/*精度*/,int type)
{
    char tmp[36];
    const char *digits="0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

    if(type&SMALL)
    {
        //用小写
        digits="0123456789abcdefghijklmnopqrstuvwxyz";
    }
    if(type&LEFT)
    {
        type &=~ZEROPAD;//既要左对齐，又要补0,则不用补0,位数不够后面是空格。
    }
    if(base<2||base>36)//任意进制，超过36则超过了上面的字符个数，没法表示了。
        return 0;
    
    //c表示补的字符是啥
    char c=(type&ZEROPAD)?'0':' ';//如果flag有左对齐，又用0来补，上面又把ZEROPAD删去了，所以用空格。
    //sign表示符号，可能为 ‘+’ ‘-’ ‘ ’（空格）
    char sign;
    if(type&SIGN &&num<0)
    {
        sign='-';
        num=-num;
    }
    else
    {
        sign=(type&PLUS)?'+':((type&SPACE)?' ':0);//num可能为0和正数，有PLUS那么输出带+，没有PLUS有空格输出带空格，没有空格那就是为0
    }

    if(sign)//如果有正负号，占去一位宽度
    {
        size--;
    }
    if(type&SPECIAL)
    {
        if(base==16)
            size-=2;//0x
        else if(base==8)
            size--;//o
    }
    int i=0;
    if(num==0)
    {
        tmp[i++]='0';
    }
    else
    {
        while(num!=0)
        {
            tmp[i++]=digits[do_div(num,base)];//do_div得到余数，并转换成字符。
        }
    }

    if(i>precision) //内核内部没有浮点数，所以这里不实现。
    {
        precision=i;//精度限制。
    }
    size-=precision;//如果表达式是带小数的整数，size剩下为整数部分。
    if(!(type&(ZEROPAD|LEFT)))
    {
        while (size-->0)
        {
            *str++=' ';
        }
    }
    if(sign)
    {
        *str++=sign;
    }
    if(type&SPECIAL)
    {
        if(base==8)
        {
            *str++='o';
        }
        else if(base==16)
        {
            *str++='0';
            *str++='x';
        }
    }
    if (!(type&LEFT)) //没有LEFT
    { 
        while (size-- > 0) 
        {
            *str++ = c;
        }
    }
    while (i < precision--) //用0补齐到指定宽度
    {
        *str++ = '0';
    }
    while (i-- > 0) //反着到给str，num25,10进制，tmp中存‘5’‘2’，现在str中变成25
    {              
        *str++ = tmp[i];
    }
    while (size-- > 0) //用空格补齐到指定宽度
    {
        *str++ = ' ';
    }

    return str;
}
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

        int flags=0;
        int size;//输出的长度
        int precision; //确定输出精度即小数点后面的位数。（实际上并没有实现浮点书）
        repeat:
            ++format;//跳过第一个%，
            switch (*format)
            {
            case '-': flags|=LEFT;
                goto repeat;
            case '+': flags|=PLUS;
                goto repeat;
            case ' ': flags|=SPACE;
                goto repeat;
            case '#': flags|=SPECIAL;
                goto repeat;
            case '0': flags|=ZEROPAD;
                goto repeat;
            default:
                break;
            }//直接是数字的话就跳到下面处理

        //获取字符长度
        size=-1;
        if(is_digit(*format))
        {
            size=skip_atoi(&format);
        }
        else if(*format=='*')//printf("%*.*f",a,b,c);表示用a和b去替代前面两个*,一个作为长度，一个作为精度。
        {
            size=va_arg(args,int);
            if(size<0)//%-10 表明左对齐
            {
                size=-size;
                flags|=LEFT;
            }
        }

        //获取精度
        precision=-1;
        if(*format=='.')
        {
            ++format;
            if(is_digit(*format))
            {
                precision=skip_atoi(&format);
            }
            else if(*format=='*')
            {
                precision=va_arg(args,int);//同上面size一样获取精度。
            }
            if(precision<0) precision=0;
        }

        // get the conversion qualifier
		//int qualifier = -1;	// 'h', 'l', or 'L' for integer fields
		if (*format == 'h' || *format == 'l' || *format == 'L') 
        {
			//qualifier = *format;
			++format;
		}//%lf,%ld，输出double浮点书和long整数。%hd，hf输出短浮点和短整形，即float和int
        char *s;
        int len;
        int *ip;
        switch (*format)
        {
        case 'c'://单个字符
            if(!(flags&LEFT))//不是左对齐，则最后输出数据
            {
                while(--size>0)
                {
                    *str++=' ';
                }
            }
            *str++=(unsigned char) va_arg(args,int);//字节对齐，取int，再强转成char
            while(--size>0)
            {
                *str++=' ';
            }

            break;
        
        case 's'://字符串
            s=va_arg(args,char*);
            len=strlen(s);
            if(precision<0)
            {
                precision=len;
            }
            else if(precision<len)
            {
                len=precision;
            }

            if(!(flags&LEFT))
            {
                while(len<size--)
                    *str++=' ';
            }
            for(uint32_t i=0;i<len;i++)
            {
                *str++=*s++;
            }
            while(len<size--)
                *str++=' ';
            break;
        
        case 'o'://8进制
            str=number(str,va_arg(args,unsigned long),8,size,precision,flags);
            break;
        
        case 'p'://打印指针
            if(size==-1)
            {
                size=8;
                flags|=ZEROPAD;
            }
            str=number(str,(unsigned long) va_arg(args, void *),16,size,precision,flags);
            break;
        
        case 'x':
            flags|=SMALL;
        case 'X':
            str=number(str,va_arg(args,unsigned long),16,size,precision,flags);
            break;
        
        case 'd'://整数
        case 'i':
            flags|=SIGN;
        case 'u'://十进制无符号整数
            str=number(str,va_arg(args,unsigned long),10,size,precision,flags);
            break;
        case 'b':
            str=number(str,va_arg(args,unsigned long),2,size,precision,flags);
            break;
        
        case 'n':
            ip=va_arg(args,int*);
            *ip=(str-buff);//???????记录输出长度吗？
            break;

        default:
            if(*format!='%')
                *str++='%';
            if(*format)
            {
                *str++=*format;
            }
            else
            {
                --format;
            }
            break;
            
        }
    }
    *str='\0';

    return (str-buff);//返回输出字符长度。
}
