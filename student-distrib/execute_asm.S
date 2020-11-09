.globl push_iret_context_asm
.globl flush_tlb
.globl go_to_exec

push_iret_context_asm:
    # popl %ecx
    # popl %eax
    #popl %ebx
    movl 4(%esp), %eax
    movl 8(%esp), %ebx
    movl 12(%esp), %ecx
    pushfl 
    orl $0x0200, (%esp)
    pushl %ebx
    pushl %eax
    iret
go_to_exec:
    ret

flush_tlb:
	movl	%cr3,%eax
	movl	%eax,%cr3
    ret

