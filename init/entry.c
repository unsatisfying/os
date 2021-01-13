#include "types.h"
#include "screen.h"
#include "string.h"
#include "debug.h"
#include "gdt.h"
#include "idt.h"
#include "timer.h"
#include "pmm.h"
#include "vmm.h"
void kern_init();
multiboot_t *glb_mboot_ptr;
char kern_stack[STACK_SIZE];

__attribute__((section(".init.data"))) pgd_t *pgd_tmp = (pgd_t *)0x1000;
__attribute__((section(".init.data"))) pgd_t *pte_low = (pgd_t *)0x2000;
__attribute__((section(".init.data"))) pgd_t *pte_hign = (pgd_t *)0x3000;

int kern_entry()
{
	init_debug();
	init_gdt();
	init_idt();
	screen_clear();

	printk_color(rc_black, rc_green, "HAPPY NEW YEAR!\n");

	init_timer(200);

	//asm volatile("sti");

	printk("Kernel start addr in memory: 0x%08X\n", kern_start);
	printk("Kernel end addr in memory: 0x%08X\n", kern_end);
	printk("Kernel used %d KB in memory!\n\n\n", (kern_end - kern_start + 1) / 1024);

	pmm_init();
	pm_alloc_re_t r_struct1, r_struct2, r_struct3, r_struct4, r_struct5;
	r_struct1 = pmm_alloc_pages(16);
	printk("get memory: addr:0X%08X  size:%d\n", r_struct1.addr, 1 << r_struct1.size);
	r_struct2 = pmm_alloc_pages(16);
	printk("get memory: addr:0X%08X  size:%d\n", r_struct2.addr, 1 << r_struct2.size);
	r_struct3 = pmm_alloc_pages(16);
	printk("get memory: addr:0X%08X  size:%d\n", r_struct3.addr, 1 << r_struct3.size);
	r_struct4 = pmm_alloc_pages(16);
	printk("get memory: addr:0X%08X  size:%d\n", r_struct4.addr, 1 << r_struct4.size);
	r_struct5 = pmm_alloc_pages(16);
	printk("get memory: addr:0X%08X  size:%d\n", r_struct4.addr, 1 << r_struct4.size);

	pmm_free_page(r_struct1);
	pmm_free_page(r_struct2);
	pmm_free_page(r_struct3);
	pmm_free_page(r_struct4);
	pmm_free_page(r_struct5);
	//asm volatile("cli");
	return 0;
}
