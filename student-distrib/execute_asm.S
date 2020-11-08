.globl push_iret_context_asm
.globl flush_tlb
.globl go_to_exec

push_iret_context_asm:
    popl %eax
    popl %ebx
    pushfl 
    popl %ecx
    orl %ecx, 0x00000100
    pushfl
    pushl %ebx
    pushl %eax
    iret
go_to_exec:
    ret


flush_tlb:
	movl	%cr3,%eax
	movl	%eax,%cr3
    ret

