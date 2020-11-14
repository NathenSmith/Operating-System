.globl push_iret_context
.globl flush_tlb
.globl go_to_exec

flush_tlb:
	movl	%cr3,%eax
	movl	%eax,%cr3
    ret

push_iret_context:
    pushl %ebp
    movl %esp, %ebp

    movl 12(%ebp), %ecx #ecx holds curr_pcb
    movl 8(%ebp), %edx #edx holds entrypoint
    movl %esp, 8(%ecx) #curr_pcb->esp = esp
    movl %ebp, 12(%ecx) #curr_pcb->ebp = ebp

    pushl $0x002B
    pushl $0x83FFFFC
    pushfl
    popl %eax
    orl $0x200, %eax
    pushl %eax
    pushl $0x0023
    pushl %edx
    
    iret
go_to_exec:
    leave
    ret
