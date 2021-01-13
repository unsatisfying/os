#include "screen.h"
#include "string.h"
// VGA缓冲在内存中的起点位置，static保证这三个变量只在此文件中使用
static uint16_t *VGA_memory = (uint16_t *)(0xB8000+PAGE_OFFSET);

//定义当前光标位置坐标,屏幕是80（字节）×25大小,一个屏幕显示2000字符
static uint8_t cur_x = 0;
static uint8_t cur_y = 0;

//改变当前位置函数
static void move_cur_location()
{
    uint16_t cur_location = cur_y * 80 + cur_x;

    //显卡的显示控制单元被编制在独立的I/O空间里，要用特殊的in/out指令去读写端口
    //通过0x3D4端口作为显卡内部寄存器的索引，通过0x3D5端口来设置相应寄存器的值
    //这里用到了14和15两个内部寄存器编号，分别表示光标的高8位和低8位。

    outb(0x3D4, 14);
    outb(0x3D5, cur_location >> 8);
    outb(0x3D4, 15);
    outb(0x3D5, cur_location);
}

//清除屏幕操作
void screen_clear()
{
    uint8_t color_byte = (0 << 4) | 0xf; //黑底白字00001111b
    uint16_t blank =
        (color_byte << 8) |
        0x20; // intel是小端方式，低字节在低地址，所以低字节放的是字节颜色属性，高字节放的是字符的ascii码，空格的ascii码是32,0x20.

    // for(int i=0;i<80*25;i++)
    // {
    //     VGA_memory[i]=blank;
    // }
    memsetw(VGA_memory, blank, 80 * 25);

    cur_x = 0;
    cur_y = 0;
    move_cur_location();
}
//当前位置等于25时候，说明要往上一行进行滚屏，把后面一行的字节拷到前面一行，最后一行填充空格。
static void screen_roll()
{

    if (cur_y >= 25)
    {
        //构造空格字符
        uint8_t color_byte = (0 << 4) | 0x0f; //黑底白字00001111b
        uint16_t blank =
            (color_byte << 8) |
            0x20; // intel是小端方式，低字节在低地址，所以低字节放的是字节颜色属性，高字节放的是字符的ascii码，空格的ascii码是32,0x20.
        // int i;
        //前24行
        for (int i = 0; i < 80 * 24; i++)
        {
            VGA_memory[i] = VGA_memory[i + 80];
        }
        //最后一行
        for (int i = 80 * 24; i < 80 * 25; i++)
        {
            VGA_memory[i] = blank;
        }

        cur_y = 24;
    }
}
//屏幕输出一个字符
void screen_putc_color(char c, real_color_t back, real_color_t fore)
{
    uint8_t back_color = (uint8_t)back;
    uint8_t fore_color = (uint8_t)fore;

    uint8_t color_byte = (back_color << 4) | fore_color;
    uint16_t c_color_byte = (color_byte << 8) | c;

    if (c == 0x08 && cur_x) //退格
    {
        --cur_x;
    }
    else if (c == 0x09) // tab
    {
        cur_x = (cur_x + 8) & (1 << 3); //保证cur_x是8字节对齐的
    }
    else if (c == '\r') // return 回到这一行的行首
    {
        cur_x = 0;
    }
    else if (
        c ==
        '\n') // newline
              // 到下一行，并回到行首（实际上\n并不会回到行首，但是这里这样处理了，方便一点，一般都是\r\n同时用。）
    {
        cur_x = 0;
        ++cur_y;
    }
    else if (c >= 0x20) //大于等于32的字符都是可显示字符
    {
        VGA_memory[cur_y * 80 + cur_x] = c_color_byte;
        ++cur_x;
    }

    if (cur_x >= 80)
    {
        cur_x = 0;
        ++cur_y;
    }

    screen_roll();       //滚屏
    move_cur_location(); //改变光标位置
}

//屏幕输出一个C风格的字符串，默认为白底黑字。
void screen_write(char *cstr)
{
    while (*cstr)
    {
        screen_putc_color(*cstr++, rc_black, rc_white);
    }
}

//屏幕输出一个C风格字符串，带颜色参数
void screen_write_color(char *cstr, real_color_t back, real_color_t fore)
{
    while (*cstr)
    {
        screen_putc_color(*cstr++, back, fore);
    }
}

//屏幕输出一个16进制整型数
void screen_write_hex(uint32_t n, real_color_t back, real_color_t fore)
{
    screen_write_color("0x", back, fore);
    int i = 28;
    while ((n >> i) == 0)
        i -= 4;
    for (; i >= 0; i -= 4)
    {
        int tmp = (n >> i) & 0xf;
        if (tmp >= 0xA)
        {
            screen_putc_color(tmp - 0xA + 'a', back, fore);
        }
        else
        {
            screen_putc_color(tmp + '0', back, fore);
        }
    }
}

//屏幕输出一个10进制整形数
void screen_write_dec(uint32_t n, real_color_t back, real_color_t fore)
{
    if (n == 0)
    {
        screen_putc_color('0', back, fore);
        return;
    }
    char c[12];
    int i = 0;
    while (n)
    {
        c[i] = '0' + n % 10;
        n /= 10;
        i++;
    }
    c[i--] = '\0';
    int j = 0;
    while (j < i)
    {
        char tmp = c[j];
        c[j] = c[i];
        c[i] = tmp;
        --i;
        ++j;
    }
    screen_write_color(c, back, fore);
}
