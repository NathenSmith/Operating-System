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

# isr_wrapper0
# Contains the assembly linkage for interrupt 0(not filled in yet).
# inputs: none
# outputs: none
# side effects: none.

isr_wrapper0:
    iret

# isr_wrapper1
# Contains the assembly linkage for the keyboard.
# inputs: none
# outputs: none
# side effects: disables interrupts during call.

isr_wrapper1:
    cli
    pushal
    call key_board_handler
    popal
    sti
    iret

# isr_wrapper2
# Contains the assembly linkage for interrupt 3(not filled in yet).
# inputs: none
# outputs: none
# side effects: none.

isr_wrapper2:
    iret

# isr_wrapper3
# Contains the assembly linkage for interrupt 4(not filled in yet).
# inputs: none
# outputs: none
# side effects: none.

isr_wrapper3:
    iret

# isr_wrapper4
# Contains the assembly linkage for interrupt 5(not filled in yet).
# inputs: none
# outputs: none
# side effects: none.

isr_wrapper4:
    iret

# isr_wrapper5
# Contains the assembly linkage for interrupt 6(not filled in yet).
# inputs: none
# outputs: none
# side effects: none.

isr_wrapper5:
    iret

# isr_wrapper6
# Contains the assembly linkage for interrupt 7(not filled in yet).
# inputs: none
# outputs: none
# side effects: none.

isr_wrapper6:
    iret

# isr_wrapper7
# Contains the assembly linkage for interrupt 8(not filled in yet).
# inputs: none
# outputs: none
# side effects: none.

isr_wrapper7:
    iret

# isr_wrapper8
# Contains the assembly linkage for the rtc.
# inputs: none
# outputs: none
# side effects: disables interrupts during call.

isr_wrapper8:
    cli
    pushal
    call rtc_handler
    popal
    sti
    iret

# isr_wrapper9
# Contains the assembly linkage for interrupt 10(not filled in yet).
# inputs: none
# outputs: none
# side effects: none.

isr_wrapper9:
    iret

# isr_wrapperA
# Contains the assembly linkage for interrupt 11(not filled in yet).
# inputs: none
# outputs: none
# side effects: none.

isr_wrapperA:
    iret

# isr_wrapperB
# Contains the assembly linkage for interrupt 12(not filled in yet).
# inputs: none
# outputs: none
# side effects: none.

isr_wrapperB:
    iret

# isr_wrapperC
# Contains the assembly linkage for interrupt 13(not filled in yet).
# inputs: none
# outputs: none
# side effects: none.

isr_wrapperC:
    iret

# isr_wrapperD
# Contains the assembly linkage for interrupt 14(not filled in yet).
# inputs: none
# outputs: none
# side effects: none.

isr_wrapperD:
    iret

# isr_wrapperE
# Contains the assembly linkage for interrupt 15(not filled in yet).
# inputs: none
# outputs: none
# side effects: none.

isr_wrapperE:
    iret

# isr_wrapperF
# Contains the assembly linkage for interrupt 16(not filled in yet).
# inputs: none
# outputs: none
# side effects: none.

isr_wrapperF:
    iret

# sys_call
# calls a specific system call using a jump table.
# inputs: none
# outputs: none
# side effects: none

sys_call:
    #stack setup
    pushl %ebp
    movl %esp, %ebp

    #checks if index is between 1 and 10
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
    jmp *jump_table(, %eax, 4) #jump table access
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
