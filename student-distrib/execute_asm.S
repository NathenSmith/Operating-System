.globl push_iret_context_asm
.globl flush_tlb
.globl go_to_exec

go_to_exec:
    leave
    ret

flush_tlb:
	movl	%cr3,%eax
	movl	%eax,%cr3
    ret

