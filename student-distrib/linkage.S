.globl   isr_wrapper20
.globl   isr_wrapper21
.globl   isr_wrapper22
.globl   isr_wrapper23
.globl   isr_wrapper24
.globl   isr_wrapper25
.globl   isr_wrapper26
.globl   isr_wrapper27
.globl   isr_wrapper28
.globl   isr_wrapper29
.globl   isr_wrapper2A
.globl   isr_wrapper2B
.globl   isr_wrapper2C
.globl   isr_wrapper2D
.globl   isr_wrapper2E
.globl   isr_wrapper2F
.globl   sys_call
.globl save_eip

save_eip:
    .long 0

# isr_wrapper20
# Contains the assembly linkage for interrupt 0(not filled in yet).
# inputs: none
# outputs: none
# side effects: none.

isr_wrapper20:
    movl %esp, save_eip
    pushal
    call pit_handler
    popal
    iret

# isr_wrapper21
# Contains the assembly linkage for the keyboard.
# inputs: none
# outputs: none
# side effects: disables interrupts during call.

isr_wrapper21:
    cli
    pushal
    call key_board_handler
    popal
    sti
    iret

# isr_wrapper22
# Contains the assembly linkage for interrupt 3(not filled in yet).
# inputs: none
# outputs: none
# side effects: none.

isr_wrapper22:
    iret

# isr_wrapper23
# Contains the assembly linkage for interrupt 4(not filled in yet).
# inputs: none
# outputs: none
# side effects: none.

isr_wrapper23:
    iret

# isr_wrapper24
# Contains the assembly linkage for interrupt 5(not filled in yet).
# inputs: none
# outputs: none
# side effects: none.

isr_wrapper24:
    iret

# isr_wrapper25
# Contains the assembly linkage for interrupt 6(not filled in yet).
# inputs: none
# outputs: none
# side effects: none.

isr_wrapper25:
    iret

# isr_wrapper26
# Contains the assembly linkage for interrupt 7(not filled in yet).
# inputs: none
# outputs: none
# side effects: none.

isr_wrapper26:
    iret

# isr_wrapper27
# Contains the assembly linkage for interrupt 8(not filled in yet).
# inputs: none
# outputs: none
# side effects: none.

isr_wrapper27:
    iret

# isr_wrapper28
# Contains the assembly linkage for the rtc.
# inputs: none
# outputs: none
# side effects: disables interrupts during call.

isr_wrapper28:
    pushal
    call rtc_handler
    popal
    iret

# isr_wrapper29
# Contains the assembly linkage for interrupt 10(not filled in yet).
# inputs: none
# outputs: none
# side effects: none.

isr_wrapper29:
    iret

# isr_wrapper2A
# Contains the assembly linkage for interrupt 11(not filled in yet).
# inputs: none
# outputs: none
# side effects: none.

isr_wrapper2A:
    iret

# isr_wrapper2B
# Contains the assembly linkage for interrupt 12(not filled in yet).
# inputs: none
# outputs: none
# side effects: none.

isr_wrapper2B:
    iret

# isr_wrapper2C
# Contains the assembly linkage for interrupt 13(not filled in yet).
# inputs: none
# outputs: none
# side effects: none.

isr_wrapper2C:
    iret

# isr_wrapper2D
# Contains the assembly linkage for interrupt 14(not filled in yet).
# inputs: none
# outputs: none
# side effects: none.

isr_wrapper2D:
    iret

# isr_wrapperE
# Contains the assembly linkage for interrupt 15(not filled in yet).
# inputs: none
# outputs: none
# side effects: none.

isr_wrapper2E:
    iret

# isr_wrapperF
# Contains the assembly linkage for interrupt 16(not filled in yet).
# inputs: none
# outputs: none
# side effects: none.

isr_wrapper2F:
    iret

# sys_call
# calls a specific system call using a jump table.
# inputs: none
# outputs: none
# side effects: none

sys_call:
    
    #checks if index is between 1 and 10
    cmpl $1, %eax
    jb invalid
    cmpl $10, %eax
    jg invalid
    addl $-1, %eax

    pushl %ebp
    pushl %esi
    pushl %edi

    pushl %edx
    pushl %ecx
    pushl %ebx
    
    call *jump_table(, %eax, 4) #jump table access

    popl %ebx
    popl %ecx
    popl %edx

    popl %edi
    popl %esi
    popl %ebp

    iret
    
jump_table:
    .long halt, execute, read, write, open, close, getargs, vidmap, set_handler, sigreturn



invalid:
    movl $-1, %eax
    iret

.end
