#include "pmm.h"
#include "common.h"
#include "debug.h"
#include "multiboot.h"
#include "screen.h"

//物理页分配的数量
uint32_t phy_page_count;

//单独页的第一个页号
uint32_t singel_page_first_no;

//页开始地址,因为页结构从2MB地址开始，512M内存一共需要2M空间来存，所以剩下的从4MB开始
// 512×1024×1024/（4096）  × 16
//          页个数        ×  每个页结构体大小 12字节
uint32_t pmm_page_start = 0x00400000;

//页结束地址
uint32_t pmm_page_end;

pm_page_t *page_array = (pm_page_t *)0x00200000;

pm_linklist_header_t *MULTI_LINK;

pm_linklist_header_t multi_link_struct;

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
	return 1 << ph;
}

static void add_block_to_link(uint32_t page_no, page_c_t c)
{
	//page_array[page_no].next = NULL;
	pm_page_t *header;
	switch (c)
	{
	case _1:
	case _2:
	case _4:
	case _8:
	case _16:
	case _32:
	case _64:
	case _128:
	case _256:
	case _512:
	case _1024:
	case _sigle:
		header = MULTI_LINK->link[c];
		if (!header)
		{
			MULTI_LINK->link[c] = &(page_array[page_no]);
			page_array[page_no].next = NULL;
			page_array[page_no].pre = NULL;
			page_array[page_no].state = 0;
			page_array[page_no].page_number = page_no;
		}
		else
		{
			pm_page_t *header_pre = header->pre;
			if (header_pre)
			{
				header_pre->next = &(page_array[page_no]);
				header->pre = &(page_array[page_no]);
				page_array[page_no].next = header;
				page_array[page_no].pre = header_pre;
			}
			else
			{
				header->next = &(page_array[page_no]);
				header_pre = &(page_array[page_no]);
				page_array[page_no].next = header;
				page_array[page_no].pre = header;
			}
		}
		MULTI_LINK->node_length[c]++;
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
	case _2:
	case _4:
	case _8:
	case _16:
	case _32:
	case _64:
	case _128:
	case _256:
	case _512:
	case _1024:
	case _sigle:
		header = MULTI_LINK->link[c];
		if (!header)
			return ERRO_POP_BLOCK;
		else if (!header->next)
		{
			MULTI_LINK->link[c] = NULL;
			MULTI_LINK->node_length[c]--;
			return header->page_number;
		}
		else
		{
			if (header->next->next == header)
			{
				pm_page_t *header_pre = header->pre;
				header_pre->next = NULL;
				header_pre->pre = NULL;
				header->next = NULL;
				header->pre = NULL;
				return_page_no = header_pre->page_number;
			}
			else
			{
				pm_page_t *header_pre = header->pre;
				pm_page_t *header_pre_pre = header_pre->pre;
				header_pre_pre->next = header;
				header->pre = header_pre_pre;
				header_pre->next = NULL;
				header_pre->pre = NULL;
				return_page_no = header_pre->page_number;
			}
			MULTI_LINK->node_length[c]--;
			return return_page_no;
		}
		break;
	case _erro:
		return ERRO_POP_BLOCK;
		break;
	}
	return ERRO_POP_BLOCK;
}
void show_memory_map()
{
	uint32_t mmap_length = glb_mboot_ptr->mmap_length;
	uint32_t mmap_addr = glb_mboot_ptr->mmap_addr;
	//uint32_t PM_MULTIBOOT_MAX_ADDR = 0;
	printk("Multiboot shows memory map:\n");
	mmap_entry_t *mmap = (mmap_entry_t *)mmap_addr;
	for (mmap = (mmap_entry_t *)mmap_addr; (uint32_t)mmap < mmap_addr + mmap_length; mmap++)
	{
		printk("base_addr = 0x%X%08X, length = 0x%X%08X, type = 0x%X\n",
			   (uint32_t)mmap->base_addr_high, (uint32_t)mmap->base_addr_low,
			   (uint32_t)mmap->length_high, (uint32_t)mmap->length_low,
			   (uint32_t)mmap->type);
		if (mmap->type == 1 && PM_MULTIBOOT_MAX_ADDR < (mmap->base_addr_low + mmap->length_low))
		{
			PM_MULTIBOOT_MAX_ADDR = mmap->base_addr_low + mmap->length_low;
		}
	}
}

static void pmm_page_init()
{
	MULTI_LINK = &multi_link_struct;
	for (int i = 0; i < 12; i++)
	{
		MULTI_LINK->link[i] = NULL;
		MULTI_LINK->node_length[i] = 0;
	}
	for (int i = 0; i < PMM_MAX_PAGE_SIZE; i++)
	{
		page_array[i].next = NULL;
		page_array[i].pre = NULL;
		page_array[i].page_number = i;
		page_array[i].state = 0; //未使用
	}

	add_block_to_link(0, _1);
	add_block_to_link(1, _1);
	add_block_to_link(2, _2);
	add_block_to_link(4, _4);
	add_block_to_link(8, _8);
	add_block_to_link(16, _16);
	add_block_to_link(32, _32);
	add_block_to_link(64, _64);
	add_block_to_link(128, _128);
	add_block_to_link(256, _256);
	add_block_to_link(512, _512);
	add_block_to_link(1024, _1024);

	int page_number_now = 2048;
	for (; page_number_now < PMM_MAX_PAGE_SIZE - 1024; page_number_now += 1024)
	{
		add_block_to_link(page_number_now, _1024);
	}

	singel_page_first_no = page_number_now;

	printk("first single page number is %d\n", page_number_now);
	printk("we have %d pages for singel page alloc!\n", PMM_MAX_PAGE_SIZE - page_number_now);
	MULTI_LINK->link[11] = &(page_array[page_number_now++]);
	MULTI_LINK->node_length[11]++;
	pm_page_t *tmpheader = MULTI_LINK->link[11];
	for (; page_number_now < PMM_MAX_PAGE_SIZE; page_number_now++)
	{
		tmpheader->next = &(page_array[page_number_now]);
		page_array[page_number_now].pre = tmpheader;
		tmpheader = tmpheader->next;
		MULTI_LINK->node_length[11]++;
	}
	tmpheader->next = MULTI_LINK->link[11];
	MULTI_LINK->link[11]->pre = tmpheader;
}
pm_alloc_re_t pmm_alloc_pages(uint32_t page_count)
{
	pm_alloc_re_t return_struct = {0, _erro, 0};
	if (page_count > 1024 || page_count == 0)
		return return_struct;
	uint32_t page_size = 0;
	while (page_count > (0x1 << page_size))
		page_size++;
	page_c_t origin_page_size = page_size;
	uint32_t pop_page_number = ERRO_POP_BLOCK;
	for (; page_size < _erro; page_size++)
	{
		pop_page_number = pop_block(page_size);
		if (pop_page_number != ERRO_POP_BLOCK)
			break;
	}
	if (pop_page_number == ERRO_POP_BLOCK)
	{
		return return_struct;
	}
	else
	{
		for (; page_size != origin_page_size; page_size--)
		{
			uint32_t add_to_link_page_number = pop_page_number + (c_to_uint32(page_size) >> 1);
			add_block_to_link(add_to_link_page_number, page_size - 1);
		}
		return_struct.addr = pmm_page_no_to_addr(pop_page_number);
		return_struct.state = 1;
		return_struct.size = origin_page_size;
		return return_struct;
	}
}
pm_alloc_re_t pmm_alloc_one_page()
{
	pm_alloc_re_t return_struct = {0, _erro, 0};
	if (MULTI_LINK->link[11])
	{
		pm_page_t *header = MULTI_LINK->link[11];
		if (header->next)
		{
			pm_page_t *header_pre = header->pre;
			if (header_pre->pre == header)
			{
				return_struct.state = 1;
				return_struct.size = 11;
				return_struct.addr = pmm_page_no_to_addr(header_pre->page_number);
				header_pre->next = NULL;
				header_pre->pre = NULL;
				header->next = NULL;
				header->pre = NULL;
			}
			else
			{
				return_struct.state = 1;
				return_struct.size = 11;
				return_struct.addr = pmm_page_no_to_addr(header_pre->page_number);
				header_pre->pre->next = header;
				header->pre = header_pre->pre;
				header_pre->next = NULL;
				header_pre->pre = NULL;
			}

			return return_struct;
		}
		else
		{
			MULTI_LINK->link[11] = NULL;
			return_struct.state = 1;
			return_struct.size = 11;
			return_struct.addr = pmm_page_no_to_addr(header->page_number);
			return return_struct;
		}
	}
	else
	{
		return pmm_alloc_pages(1);
	}
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
static int pop_block_from_MULTILINK(uint32_t page_num, page_c_t size)
{
	if (size >= 11)
		return 0;
	pm_page_t *header = MULTI_LINK->link[size];
	if (!header)
		return 0;
	if (header->page_number == page_num)
	{
		pm_page_t *header_pre = header->pre;
		pm_page_t *header_next = header->next;
		if (MULTI_LINK->node_length[size] == 1)
		{
			MULTI_LINK->link[size] = NULL;
		}
		else if (MULTI_LINK->node_length[size] == 2)
		{
			MULTI_LINK->link[size] = header_next;
			header_next->pre = NULL;
			header_next->next = NULL;
		}
		else
		{
			MULTI_LINK->link[size] = header_next;
			header_next->pre = header_pre;
			header_pre->next = header_next;
		}
		header->next = NULL;
		header->pre = NULL;
		header->state = 1;
		return 1;
	}
	else
	{
		header = header->next;
		if (MULTI_LINK->node_length[size] == 1)
		{
			return 0;
		}
		else if (MULTI_LINK->node_length[size] == 2)
		{
			if (header->page_number == page_num)
			{
				pm_page_t *header_pre = header->pre;
				header_pre->next = NULL;
				header_pre->pre = NULL;
				header->next = NULL;
				header->pre = NULL;
				header->state = 1;
				return 1;
			}
			else
			{
				return 0;
			}
		}
		else
		{
			while (header != MULTI_LINK->link[size] && header->page_number != page_num)
				header = header->next;
			if (header == MULTI_LINK->link[size])
				return 0;
			else if (header->page_number == page_num)
			{
				pm_page_t *header_pre = header->pre;
				pm_page_t *header_next = header->next;
				header_next->pre = header_pre;
				header_pre->next = header_next;
				header->next = NULL;
				header->pre = NULL;
				header->state = 1;
				return 1;
			}
			return 0;
		}
	}
}
int pmm_free_page(pm_alloc_re_t block_disk)
{
	uint32_t page_num = addr_to_pmm_page_no(block_disk.addr);
	if (page_num < PMM_MAX_PAGE_SIZE)
	{
		if (block_disk.size == _sigle)
		{
			add_block_to_link(page_num, _sigle);
			printk(" free page: page_num:%d, size:_single\n", page_num);
			return 1;
		}
		else
		{
			uint32_t partner_page_no = get_partner_page_no(page_num, block_disk.size);
			page_c_t msize = block_disk.size;
			uint32_t page_head_num = partner_page_no < page_num ? partner_page_no : page_num;
			if (!pop_block_from_MULTILINK(partner_page_no, msize))
			{
				add_block_to_link(page_num, msize);
				printk(" free page: page_num:%d, size:%d\n", page_num, 1 << msize);
				return 1;
			}
			uint32_t flag = 1;
			while (flag && msize < 10)
			{
				msize++;
				partner_page_no = get_partner_page_no(page_head_num, msize);
				page_head_num = partner_page_no < page_head_num ? partner_page_no : page_head_num;
				flag = pop_block_from_MULTILINK(partner_page_no, msize);
			}
			add_block_to_link(page_head_num, msize);
			printk(" free page: page_num:%d, size:%d\n", page_num, 1 << msize);
			return 1;
		}
	}
	return 0;
}

void pmm_init()
{
	show_memory_map();
	pmm_page_init();
}
