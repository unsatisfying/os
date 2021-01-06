#ifndef _INCLUDE_SCREEN_H_
#define _INCLUDE_SCREEN_H_

#include "common.h"
// KRGB	IRGB
// （8位，K=0不闪烁，K=1闪烁，前景色RGB从0b000-0b111，后景色IRGB从0b0000-0b1111,其中前面8个颜色和前景色相同）
typedef enum real_color
{
  rc_black = 0,          //黑色
  rc_blue = 1,           //蓝色
  rc_green = 2,          //绿色
  rc_cyan = 3,           //青色
  rc_red = 4,            //红色
  rc_magenta = 5,        //品红色
  rc_brown = 6,          //棕色
  rc_white = 7,          //白色
  rc_light_grey = 8,     //灰色
  rc_light_blue = 9,     //浅蓝色
  rc_light_green = 10,   //浅绿色
  rc_light_cyan = 11,    //浅青色
  rc_light_red = 12,     //浅红色
  rc_light_magenta = 13, //浅品红色
  rc_light_brown = 14,   //黄色
  rc_light_white = 15    //亮白色
} real_color_t;

//清除屏幕操作
void screen_clear();

//屏幕输出一个字符
void screen_putc_color(char c, real_color_t back, real_color_t fore);

//屏幕输出一个C风格的字符串，默认为白底黑字。
void screen_write(char *cstr);

//屏幕输出一个C风格字符串，带颜色参数
void screen_write_color(char *cstr, real_color_t back, real_color_t fore);

//屏幕输出一个16进制整型数
void screen_write_hex(uint32_t n, real_color_t back, real_color_t fore);

//屏幕输出一个10禁止整形数
void screen_write_dec(uint32_t n, real_color_t back, real_color_t fore);
#endif //_INCLUDE_SCREEN_H_