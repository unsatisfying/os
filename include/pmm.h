#ifndef _INCLUDE_PMM_H_
#define _INCLUDE_PMM_H_

#include "multiboot.h"
#include "types.h"

//最大物理内存512MB
#define PMM_MAX_SIZE 0x20000000

//物理页框大小4KB
#define PMM_PAGE_SIZE 0x1000

//最大可分配的物理页数
#define PMM_MAX_PAGE_SIZE (PMM_MAX_SIZE / PMM_PAGE_SIZE)

//物理页掩码，用于4096对齐
#define PHY_PAGE_MASK 0xFFFFF000

#define ERRO_POP_BLOCK 0xFFFFFFFF

//枚举分配块大小到链表索引的转换
typedef enum page_c_t
{
  _1 = 0,
  _2 = 1,
  _4,
  _8,
  _16,
  _32,
  _64,
  _128,
  _256,
  _512,
  _1024,
  _erro
} page_c_t;

//链表中一个个节点，也是每个块的相关数据结构体
typedef struct pm_page_t
{
  uint32_t page_number;
  struct pm_page_t *next;
  uint8_t state;
} pm_page_t;

//伙伴的链表集合
typedef struct pm_linklist_header_t
{
  pm_page_t *_1;
  pm_page_t *_2;
  pm_page_t *_4;
  pm_page_t *_8;
  pm_page_t *_16;
  pm_page_t *_32;
  pm_page_t *_64;
  pm_page_t *_128;
  pm_page_t *_256;
  pm_page_t *_512;
  pm_page_t *_1024;
} pm_linklist_header_t;

//分配页时候返回结构体
typedef struct pm_alloc_re_t
{
  uint32_t addr; //分配的地址
  page_c_t size; //分配页的数量级
  uint8_t state; //分配状态

} pm_alloc_re_t;

// 内核文件在内存中的起始和结束位置
// 在链接器脚本中要求链接器定义
extern uint8_t kern_start[];
extern uint8_t kern_end[];
// 输出 BIOS 提供的物理内存布局

void show_memory_map();
void pmm_init();
pm_alloc_re_t pmm_alloc_pages(uint32_t);
pm_alloc_re_t pmm_alloc_one_page();
int pmm_free_page(pm_alloc_re_t);
#endif //_INCLUDE_PRINTMM_H_