# uses C calling convention
push_iret_context:
    popl %eax
    popl %ebx
    pushfl 
    popfl %ecx
    orl %ecx, 0x00000100
    pushfl
    pushl %ebx
    pushl %eax
    iret

flush_tlb:
	movl	%cr3,%eax
	movl	%eax,%cr3
    ret
