#include "gdt.h"
#include "string.h"

#define GDT_LENGTH 5//全局描述符表长度

gdt_entry_t gdt_entries[GDT_LENGTH];//全局描述符表数组

gdt_ptr_t gdt_ptr;//GDTR寄存器48位结构，高32位为基地址，低32位为全局描述符表的边界

// 全局描述符表构造函数，根据下标构造
//num 下标志    
//base 段基地址 0
//limit 段界限  32位 0xffffffff
//access 段存在位、描述符特权级、描述符类型、描述符子类别 
//gran 其他标志、段界限 19～16
static void gdt_set_gate(int32_t num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran)
{
    gdt_entries[num].base_low = (base & 0xFFFF);
    gdt_entries[num].base_middle = (base >> 16) & 0xFF;
    gdt_entries[num].base_high = (base >> 24) & 0xFF;

    gdt_entries[num].limit_low = (limit & 0xFFFF);
    gdt_entries[num].granularity = (limit >> 16) & 0x0F;

    gdt_entries[num].granularity |= gran & 0xF0;
    gdt_entries[num].access = access;
}



//内核栈地址
extern uint32_t stack;

// 初始化全局描述符表
void init_gdt()
{
    gdt_ptr.limit=sizeof(gdt_entry_t)*GDT_LENGTH-1;//即gdt_entries的长度-1,因为下标从0开始。
    gdt_ptr.base=(uint32_t) &gdt_entries;//取表的地址

    //intel平坦内存模型
    gdt_set_gate(0,0,0,0,0);//intel文档要求第一个描述符必须全0
    gdt_set_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF); // 指令段   10011010
    gdt_set_gate(2, 0, 0xFFFFFFFF, 0x92, 0xCF); // 数据段   10010010
    gdt_set_gate(3, 0, 0xFFFFFFFF, 0xFA, 0xCF); // 用户模式代码段   11111010
    gdt_set_gate(4, 0, 0xFFFFFFFF, 0xF2, 0xCF); // 用户模式数据段   11110010

    // 加载全局描述符表地址到 GPTR 寄存器
    gdt_flush((uint32_t)&gdt_ptr);
}

// GDT 加载到 GDTR 的函数[汇编实现]
extern void gdt_flush(uint32_t);