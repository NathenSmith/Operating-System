.globl flush_tlb
.globl go_to_exec
.globl push_iret_context_test
.globl restore_parent_data

restore_parent_data:
    #stack setup
    pushl %ebp
    movl %esp, %ebp    

    #get args
    movl 8(%ebp), %ebx # curr_pcb
    movl 12(%ebp), %eax # status

    #restore esp and ebp
    movl 8(%ebx), %esp
    movl 12(%ebx), %ebp

    jmp go_to_exec

flush_tlb:
    #stack setup
    pushl %ebp
    movl %esp, %ebp

    #push callee saved registers to stack
    pushl %esi
    pushl %edi
    pushl %ebx

	movl	%cr3,%eax
	movl	%eax,%cr3
    
    #restore callee saved registers
    popl %ebx
    popl %edi
    popl %esi

    leave
    ret

push_iret_context_test:
    #stack setup
    pushl %ebp
    movl %esp, %ebp

    #push callee saved registers to stack
    pushl %esi
    pushl %edi
    pushl %ebx

    #get curr_pcb
    movl 8(%ebp), %ebx #curr_pcb

    #save esp and ebp
    movl %esp, 8(%ebx)
    movl %ebp, 12(%ebx)

    #get ss and esp and push to stack
    movl 24(%ebp), %ebx #ss
    movl 20(%ebp), %ecx #esp
    pushl %ebx
    pushl %ecx

    #push and modify flags
    pushfl
    popl %eax 
    orl $0x200, %eax
    pushl %eax

    #get eip and cs and push to stack
    movl 16(%ebp), %ebx #cs
    movl 12(%ebp), %ecx #eip
    pushl %ebx
    pushl %ecx

    iret

go_to_exec:
    #restore callee saved registers
    popl %ebx
    popl %edi
    popl %esi

    leave
    ret

