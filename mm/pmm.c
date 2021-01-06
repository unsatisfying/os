#include "pmm.h"
#include "common.h"
#include "debug.h"
#include "multiboot.h"
#include "screen.h"

//物理页分配的数量
uint32_t phy_page_count;

//单独页的第一个页号
uint32_t singel_page_first_no;

//页开始地址,因为页结构从2MB地址开始，512M内存一共需要1.5M空间来存，所以剩下的从4MB开始，中间空0.5M留做他用
// 512×1024×1024/（4096）  × 12
//          页个数        ×  每个页结构体大小 12字节
uint32_t pmm_page_start = 0x00400000;

//页结束地址
uint32_t pmm_page_end;

pm_page_t *page_array = (pm_page_t *)0x00200000;

pm_linklist_header_t *MULTI_LINK;

pm_linklist_header_t multi_link_struct = {NULL, NULL, NULL, NULL, NULL, NULL,
										  NULL, NULL, NULL, NULL, NULL};

pm_page_t *SINGLE_LINK = NULL;

//从multiboot中读出可用内存的最大值
uint32_t PM_MULTIBOOT_MAX_ADDR = 0;
//获取page编号对应的addr
static uint32_t pmm_page_no_to_addr(uint32_t page_no)
{
	return (page_no << 12) + pmm_page_start;
}

// addr转为page_no
static uint32_t addr_to_pmm_page_no(uint32_t addr)
{
	return (addr - pmm_page_start) >> 12;
}

//将 page_c_t枚举类型转化为对应的块页面个数 如 (page_c_t)_256——> (uint32_t)256
static uint32_t c_to_uint32(page_c_t ph)
{
	uint32_t re = 1;
	re = re << ph;
	return re;
}
//获得伙伴数组的页号
static uint32_t get_partner_page_no(uint32_t page_no, page_c_t type)
{
	uint32_t v1 = c_to_uint32(type);
	uint32_t v2 = v1 << 1;
	if ((page_no - v1) % v2 == 0)
		return page_no - v1;
	else
		return page_no + v1;
}

static void add_block_to_link(uint32_t page_no, page_c_t c)
{
	page_array[page_no].next = NULL;
	pm_page_t *header;
	switch (c)
	{
	case _1:
		header = MULTI_LINK->_1;
		if (!header)
			MULTI_LINK->_1 = &(page_array[page_no]);
		else
		{
			while (header->next)
				header = header->next;
			header->next = &(page_array[page_no]);
		}
		break;
	case _2:
		header = MULTI_LINK->_2;
		if (!header)
			MULTI_LINK->_2 = &(page_array[page_no]);
		else
		{
			while (header->next)
				header = header->next;
			header->next = &(page_array[page_no]);
		}
		break;
	case _4:
		header = MULTI_LINK->_4;
		if (!header)
			MULTI_LINK->_4 = &(page_array[page_no]);
		else
		{
			while (header->next)
				header = header->next;
			header->next = &(page_array[page_no]);
		}
		break;
	case _8:
		header = MULTI_LINK->_8;
		if (!header)
			MULTI_LINK->_8 = &(page_array[page_no]);
		else
		{
			while (header->next)
				header = header->next;
			header->next = &(page_array[page_no]);
		}
		break;
	case _16:
		header = MULTI_LINK->_16;
		if (!header)
			MULTI_LINK->_16 = &(page_array[page_no]);
		else
		{
			while (header->next)
				header = header->next;
			header->next = &(page_array[page_no]);
		}
		break;
	case _32:
		header = MULTI_LINK->_32;
		if (!header)
			MULTI_LINK->_32 = &(page_array[page_no]);
		else
		{
			while (header->next)
				header = header->next;
			header->next = &(page_array[page_no]);
		}
		break;
	case _64:
		header = MULTI_LINK->_64;
		if (!header)
			MULTI_LINK->_64 = &(page_array[page_no]);
		else
		{
			while (header->next)
				header = header->next;
			header->next = &(page_array[page_no]);
		}
		break;
	case _128:
		header = MULTI_LINK->_128;
		if (!header)
			MULTI_LINK->_128 = &(page_array[page_no]);
		else
		{
			while (header->next)
				header = header->next;
			header->next = &(page_array[page_no]);
		}
		break;
	case _256:
		header = MULTI_LINK->_256;
		if (!header)
			MULTI_LINK->_256 = &(page_array[page_no]);
		else
		{
			while (header->next)
				header = header->next;
			header->next = &(page_array[page_no]);
		}
		break;
	case _512:
		header = MULTI_LINK->_512;
		if (!header)
			MULTI_LINK->_512 = &(page_array[page_no]);
		else
		{
			while (header->next)
				header = header->next;
			header->next = &(page_array[page_no]);
		}
		break;
	case _1024:
		header = MULTI_LINK->_1024;
		if (!header)
			MULTI_LINK->_1024 = &(page_array[page_no]);
		else
		{
			while (header->next)
				header = header->next;
			header->next = &(page_array[page_no]);
		}
		break;
	case _erro:
		break;
	}
}

static uint32_t pop_block(page_c_t c)
{
	pm_page_t *header;
	uint32_t return_page_no;
	switch (c)
	{
	case _1:
		header = MULTI_LINK->_1;
		if (!header)
			return ERRO_POP_BLOCK;
		else if (!header->next)
		{
			MULTI_LINK->_1 = NULL;
			return header->page_number;
		}
		else
		{
			while (header->next->next)
				header = header->next;
			return_page_no = header->next->page_number;
			header->next = NULL;
			return return_page_no;
		}
		break;
	case _2:
		header = MULTI_LINK->_2;
		if (!header)
			return ERRO_POP_BLOCK;
		else if (!header->next)
		{
			MULTI_LINK->_2 = NULL;
			return header->page_number;
		}
		else
		{
			while (header->next->next)
				header = header->next;
			return_page_no = header->next->page_number;
			header->next = NULL;
			return return_page_no;
		}
		break;
	case _4:
		header = MULTI_LINK->_4;
		if (!header)
			return ERRO_POP_BLOCK;
		else if (!header->next)
		{
			MULTI_LINK->_4 = NULL;
			return header->page_number;
		}
		else
		{
			while (header->next->next)
				header = header->next;
			return_page_no = header->next->page_number;
			header->next = NULL;
			return return_page_no;
		}
		break;
	case _8:
		header = MULTI_LINK->_8;
		if (!header)
			return ERRO_POP_BLOCK;
		else if (!header->next)
		{
			MULTI_LINK->_8 = NULL;
			return header->page_number;
		}
		else
		{
			while (header->next->next)
				header = header->next;
			return_page_no = header->next->page_number;
			header->next = NULL;
			return return_page_no;
		}
		break;
	case _16:
		header = MULTI_LINK->_16;
		if (!header)
			return ERRO_POP_BLOCK;
		else if (!header->next)
		{
			MULTI_LINK->_16 = NULL;
			return header->page_number;
		}
		else
		{
			while (header->next->next)
				header = header->next;
			return_page_no = header->next->page_number;
			header->next = NULL;
			return return_page_no;
		}
		break;
	case _32:
		header = MULTI_LINK->_32;
		if (!header)
			return ERRO_POP_BLOCK;
		else if (!header->next)
		{
			MULTI_LINK->_32 = NULL;
			return header->page_number;
		}
		else
		{
			while (header->next->next)
				header = header->next;
			return_page_no = header->next->page_number;
			header->next = NULL;
			return return_page_no;
		}
		break;
	case _64:
		header = MULTI_LINK->_64;
		if (!header)
			return ERRO_POP_BLOCK;
		else if (!header->next)
		{
			MULTI_LINK->_64 = NULL;
			return header->page_number;
		}
		else
		{
			while (header->next->next)
				header = header->next;
			return_page_no = header->next->page_number;
			header->next = NULL;
			return return_page_no;
		}
		break;
	case _128:
		header = MULTI_LINK->_128;
		if (!header)
			return ERRO_POP_BLOCK;
		else if (!header->next)
		{
			MULTI_LINK->_128 = NULL;
			return header->page_number;
		}
		else
		{
			while (header->next->next)
				header = header->next;
			return_page_no = header->next->page_number;
			header->next = NULL;
			return return_page_no;
		}
		break;
	case _256:
		header = MULTI_LINK->_256;
		if (!header)
			return ERRO_POP_BLOCK;
		else if (!header->next)
		{
			MULTI_LINK->_256 = NULL;
			return header->page_number;
		}
		else
		{
			while (header->next->next)
				header = header->next;
			return_page_no = header->next->page_number;
			header->next = NULL;
			return return_page_no;
		}
		break;
	case _512:
		header = MULTI_LINK->_512;
		if (!header)
			return ERRO_POP_BLOCK;
		else if (!header->next)
		{
			MULTI_LINK->_512 = NULL;
			return header->page_number;
		}
		else
		{
			while (header->next->next)
				header = header->next;
			return_page_no = header->next->page_number;
			header->next = NULL;
			return return_page_no;
		}
		break;
	case _1024:
		header = MULTI_LINK->_1024;
		if (!header)
			return ERRO_POP_BLOCK;
		else if (!header->next)
		{
			MULTI_LINK->_1024 = NULL;
			return header->page_number;
		}
		else
		{
			while (header->next->next)
				header = header->next;
			return_page_no = header->next->page_number;
			header->next = NULL;
			return return_page_no;
		}
		break;
	case _erro:
		return ERRO_POP_BLOCK;
		break;
	}
}
void pmm_init();
pm_alloc_re_t pmm_alloc_pages(uint32_t);
pm_alloc_re_t pmm_alloc_one_page();
int pmm_free_page(pm_alloc_re_t);
void show_memory_map()
{
	uint32_t mmap_length = glb_mboot_ptr->mmap_length;
	uint32_t mmap_addr = glb_mboot_ptr->mmap_addr;
	//uint32_t PM_MULTIBOOT_MAX_ADDR = 0;
	printk("Multiboot shows memory map:\n");
	mmap_entry_t *mmap = (mmap_entry_t *)mmap_addr;
	int i = 0;
	for (mmap = (mmap_entry_t *)mmap_addr; (uint32_t)mmap < mmap_addr + mmap_length; mmap++)
	{
		printk("base_addr = 0x%X%08X, length = 0x%X%08X, type = 0x%X\n",
			   (uint32_t)mmap->base_addr_high, (uint32_t)mmap->base_addr_low,
			   (uint32_t)mmap->length_high, (uint32_t)mmap->length_low,
			   (uint32_t)mmap->type);
		if (mmap->type == 1 && PM_MULTIBOOT_MAX_ADDR < mmap->base_addr_low + mmap->length_low)
		{
			PM_MULTIBOOT_MAX_ADDR < mmap->base_addr_low + mmap->length_low;
		}
	}
}

static void pmm_page_init()
{
	MULTI_LINK = &multi_link_struct;
}
