.globl flush_tlb
.globl go_to_exec
.globl push_iret_context_test
.globl restore_parent_data

restore_parent_data:
    #stack setup
    pushl %ebp
    movl %esp, %ebp    

    #get args
    movl 8(%ebp), %ecx # curr_pcb->esp
    movl 12(%ebp), %ebx # curr_pcb->ebp
    movl 16(%ebp), %eax # status

    #restore esp and ebp
    movl %ecx, %esp
    movl %ebx, %ebp

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

    movl 8(%ebp), %ebx # address of curr_pcb->parent_ptr->esp
    movl 12(%ebp), %ecx # address of curr_pcb->parent_ptr->ebp    

    addl $-8, %ebx
    cmpl $0, %ebx
    je after_save
    addl $8, %ebx

    # save esp and ebp into parent pcb
    movl %esp, 0(%ebx)
    movl %ebp, 0(%ecx)

after_save:

    #get ss and esp and push to stack
    movl 28(%ebp), %ebx #ss
    movl 24(%ebp), %ecx #esp
    pushl %ebx
    pushl %ecx

    #push and modify flags
    pushfl
    popl %eax 
    orl $0x200, %eax
    pushl %eax

    #get eip and cs and push to stack
    movl 20(%ebp), %ebx #cs
    movl 16(%ebp), %ecx #eip
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

