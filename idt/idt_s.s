[GLOBAL idt_flush]
idt_flush:
    mov eax,[esp+4] ;参数存入eax寄存器
    lidt [eax]  ;把eax地址下的内容传入IDTR
    ret
.end




;用宏定义两个构造中断处理函数的宏
;没有错误代码的中断，要手动push一个错误代码0,填充结构体用

%macro ISR_NOERRCODE 1
[GLOBAL isr%1]
isr%1:
        cli;关中断
        push 0;填充假的中断错误码0
        push %1;填充中断错误号
        jmp isr_common_stub
%endmacro


;有错误代码的中断
%macro ISR_ERRCODE 1
[GLOBAL isr%1]
isr%1:
        cli;
        push %1;
        jmp isr_common_stub
%endmacro

; 定义中断处理函数
ISR_NOERRCODE  0 	; 0 #DE 除 0 异常
ISR_NOERRCODE  1 	; 1 #DB 调试异常
ISR_NOERRCODE  2 	; 2 NMI
ISR_NOERRCODE  3 	; 3 BP 断点异常 
ISR_NOERRCODE  4 	; 4 #OF 溢出 
ISR_NOERRCODE  5 	; 5 #BR 对数组的引用超出边界 
ISR_NOERRCODE  6 	; 6 #UD 无效或未定义的操作码 
ISR_NOERRCODE  7 	; 7 #NM 设备不可用(无数学协处理器) 
ISR_ERRCODE    8 	; 8 #DF 双重故障(有错误代码) 
ISR_NOERRCODE  9 	; 9 协处理器跨段操作
ISR_ERRCODE   10 	; 10 #TS 无效TSS(有错误代码) 
ISR_ERRCODE   11 	; 11 #NP 段不存在(有错误代码) 
ISR_ERRCODE   12 	; 12 #SS 栈错误(有错误代码) 
ISR_ERRCODE   13 	; 13 #GP 常规保护(有错误代码) 
ISR_ERRCODE   14 	; 14 #PF 页故障(有错误代码) 
ISR_NOERRCODE 15 	; 15 CPU 保留 
ISR_NOERRCODE 16 	; 16 #MF 浮点处理单元错误 
ISR_ERRCODE   17 	; 17 #AC 对齐检查 
ISR_NOERRCODE 18 	; 18 #MC 机器检查 
ISR_NOERRCODE 19 	; 19 #XM SIMD(单指令多数据)浮点异常

; 20~31 Intel 保留
ISR_NOERRCODE 20
ISR_NOERRCODE 21
ISR_NOERRCODE 22
ISR_NOERRCODE 23
ISR_NOERRCODE 24
ISR_NOERRCODE 25
ISR_NOERRCODE 26
ISR_NOERRCODE 27
ISR_NOERRCODE 28
ISR_NOERRCODE 29
ISR_NOERRCODE 30
ISR_NOERRCODE 31
; 32～255 用户自定义
ISR_NOERRCODE 255


[GLOBAL isr_common_stub]
[EXTERN isr_handler]
;中断服务程序
isr_common_stub:
        pusha   ;push edi,esi,ebp,esp,ebx,edx,ecx,eax
        mov ax,ds   ;
        push eax    ;保存数据段描述符ds

        mov ax,0x10 ;加载内核数据段的描述符表，16字节偏移，因为全局就一个段，所以基地址为0,段描述符大小一个是8字节
        mov ds,ax
        mov es,ax
        mov fs,ax
        mov gs,ax
        mov ss,ax

        push esp  ;此时ESP指针往上就是pt_regs结构，

        call isr_handler    ;此时已经保存了现场，调用中断处理函数
        add esp,4   ;//有点奇怪这里为什么会多一个参数

        pop ebx ;弹出原来的ds值
        mov ds,bx
        mov es,bx
        mov fs,bx
        mov gs,bx
        mov ss,bx

        popa    ;弹出edi -eax

        add esp ,8  ;清楚栈内错误代码和ISR中断号。
        iret
.end:


