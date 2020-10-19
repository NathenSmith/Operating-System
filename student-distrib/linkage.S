.globl   isr_wrapper0
.globl   isr_wrapper1
.globl   isr_wrapper2
.globl   isr_wrapper3
.globl   isr_wrapper4
.globl   isr_wrapper5
.globl   isr_wrapper6
.globl   isr_wrapper7
.globl   isr_wrapper8
.globl   isr_wrapper9
.globl   isr_wrapperA
.globl   isr_wrapperB
.globl   isr_wrapperC
.globl   isr_wrapperD
.globl   isr_wrapperE
.globl   isr_wrapperF
.globl   sys_call
.align   4
 
isr_wrapper0:
    cli
    pushal /* might be pushal/ popal, check do irq, push ones not covered by pushal*/
    pushfl
     /* C code following the sysV ABI requires DF to be clear on function entry */
    call rtc_handler
    popal
    popfl
    sti
    iret
isr_wrapper1:
    cli
    pushal /* might be pushal/ popal, check do irq, push ones not covered by pushal*/
    pushfl
     /* C code following the sysV ABI requires DF to be clear on function entry */
    call keyboard_handler
    popal
    popfl
    sti
    iret
isr_wrapper2:
    iret
isr_wrapper3:
    iret
isr_wrapper4:
    iret
isr_wrapper5:
    iret
isr_wrapper6:
    iret
isr_wrapper7:
    iret
isr_wrapper8:
    iret
isr_wrapper9:
    iret
isr_wrapperA:
    iret
isr_wrapperB:
    iret
isr_wrapperC:
    iret
isr_wrapperD:
    iret
isr_wrapperE:
    iret
isr_wrapperF:
    iret

sys_call:
    pushl %ebp
    movl %esp, %ebp
    cmpl $1, %eax
    jb invalid
    cmpl $10, %eax
    jg invalid

    addl $-1, %eax
    
    pushl %ecx
    pushl %edx
    pushl %ebx
    pushl %esp
    pushl %ebp
    pushl %esi
    pushl %edi
    jmp *jump_table(, %ecx, 4) #jump table access
    popl %edi
    popl %esi
    popl %ebp
    popl %esp
    popl %ebx
    popl %edx
    popl %ecx

    leave
    iret
    
jump_table:
    .long halt, execute, read, write, open, close, getargs, vidmap, set_handler, sigreturn

invalid:
    movl $-1, %eax
    leave
    iret

.end
