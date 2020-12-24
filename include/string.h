#ifndef _INCLUDE_STRING_H_
#define _INCLUDE_STRING_H_

#include "types.h"

void *memcpy(void *dst, const void *src, size_t n);
void *memset(void *dst, uint8_t c, uint32_t n);
void *memsetw(void *dst, uint16_t c, uint32_t n);
void bzero(void *dst, uint32_t n);
int strcmp(const char *str1, const char *str2);
char *strcpy(char *dst, const char *src);
char *strcat(char *dst, const char *src);
uint32_t strlen(const char *str);

#endif //_INCLUDE_STRING_H_