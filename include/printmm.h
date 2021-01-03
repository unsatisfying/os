#ifndef _INCLUDE_PRINTMM_H_
#define _INCLUDE_PRINTMM_H_

#include "multiboot.h"

// 内核文件在内存中的起始和结束位置
// 在链接器脚本中要求链接器定义
extern uint8_t kern_start[];
extern uint8_t kern_end[];
// 输出 BIOS 提供的物理内存布局

void show_memory_map();

#endif //_INCLUDE_PRINTMM_H_