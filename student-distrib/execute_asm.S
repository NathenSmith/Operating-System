.globl push_iret_context_asm
.globl flush_tlb
.globl go_to_exec

go_to_exec:
    addl $36, %esp #pop 5 arguments and 4 local variables
    ret

flush_tlb:
	movl	%cr3,%eax
	movl	%eax,%cr3
    ret

