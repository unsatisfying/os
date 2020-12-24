#include "common.h"
//汇编格式	__asm__　__volatile__("Instruction List" : Output : Input : Clobber/Modify);

inline void outb(uint16_t port, uint8_t value)
{
	__asm__ __volatile__ ("outb %1, %0" : : "dN" (port), "a" (value));
}
// IN AL,21H；表示从21H端口读取一字节数据到AL
inline uint8_t inb(uint16_t port)
{
	uint8_t ret;
	__asm__ __volatile__ ("inb %1, %0" : "=a" (ret) : "dN" (port));
	return ret;
}

inline uint16_t inw(uint16_t port)
{
	uint16_t ret;
	__asm__ __volatile__ ("inw %1, %0" : "=a" (ret) : "dN" (port));
	return ret;
}