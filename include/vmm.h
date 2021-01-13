#ifndef _INCLUDE_VMM_H_
#define _INCLUDE_VMM_H_

#include "types.h"
#include "idt.h"

#define PAGE_OFFSET 0xC0000000


#define PAGE_PRESENT 0x1
#define PAGE_WRITE 0x2

#define PAGE_USER 0x4
#define PAGE_SIZE 4096
#define PAGE_MASK 0xFFFFF000

//页目录项
#define PGD_INDEX(x) (((x) >> 22) & 0x3FF)
//页表项
#define PTE_INDEX(x) (((x) >> 12) & 0x3FF)
//页内偏移
#define OFFSET_INDEX(x) ((x)&0xFFF)

typedef uint32_t pgd_t; //32位
typedef uint32_t pte_t; //32位

#define PGD_SIZE (PAGE_SIZE/sizeof(pgd_t))
#define PTE_SIZE (PAGE_SIZE/sizeof(pte_t))

#define PTE_COUNT 131072    //512MB/4096B

extern pgd_t pgd_kern[PGD_SIZE];

void init_vmm();

void switch_pgd(uint32_t pd);

// 使用 flags 指出的页权限，把物理地址 pa 映射到虚拟地址 va
void map(pgd_t *pgd_now, uint32_t va, uint32_t pa, uint32_t flags);

// 取消虚拟地址 va 的物理映射
void unmap(pgd_t *pgd_now, uint32_t va);
// 如果虚拟地址 va 映射到物理地址则返回 1
// 同时如果 pa 不是空指针则把物理地址写入 pa 参数
uint32_t get_mapping(pgd_t *pgd_now, uint32_t va, uint32_t *pa);

//页错误终端的函数
void page_fault(pt_regs *regs);

#endif //_INCLUDE_VMM_H