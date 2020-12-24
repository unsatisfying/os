#ifndef _INCLUDE_COMMON_H_
#define _INCLUDE_COMMON_H_

#include "types.h"
//输出到端口1个byte  （1个字节，8bit）
void outb(uint16_t port, uint8_t value);

//从端口读一个byte返回
uint8_t inb(uint16_t port);

//从端口读两个byte返回
uint16_t inw(uint16_t port);

#endif //_INCLUDE_COMMON_H_