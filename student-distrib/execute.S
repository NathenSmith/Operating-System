push_iret_context:
    pushl eip
    pushl cs
    pushf
    pushl esp
    pushl ss
    iret

flush_tlb:
	movl	%cr3,%eax
	movl	%eax,%cr3
    ret
