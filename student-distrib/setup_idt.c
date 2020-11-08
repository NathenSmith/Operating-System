#include "x86_desc.h"
#include "lib.h"
#include "linkage.h"
//printk(KERN_INFO "Message: %s\n", arg);

void setup_idt_entry(void * handler_address, int irq_num, int privilege);
void system_call_handler();
void divide();    
void debug();    
void nmi();    
void breakpoint();    
void overflow();    
void boundexceed();    
void invalidopcode();    
void decidenot();
void doublefault();    
void invalidtss();    
void segmentnot();    
void stacksegfault(); 
void genprotfault();   
void pagefault();  
void floatingpoint();
void alignment_c();
void machine_c();
void simd();
void virtual_e();
void security();

/* setup_idt_entry
 * 
 * Sets up a single idt entry by setting the location of 
 the handler, size, privilege level, segment selector, 
 whether or not the handler is present, and the reserved 
 bits. 
 * Inputs: None
 * Outputs: None
 * Side Effects: None
 */

void setup_idt_entry(void * handler_address, int irq_num, int privilege)
{   
    idt[irq_num].seg_selector = KERNEL_CS;
    idt[irq_num].dpl = privilege;
    idt[irq_num].size = 1;
    idt[irq_num].present = 1;
    idt[irq_num].reserved4 = 0; 
    // 0 size 1 2 3
    // TRAP: 0 D 1 1 1
    // INTERRUPT: 0 D 1 1 0
    idt[irq_num].reserved3 = 1;
    idt[irq_num].reserved2 = 1;
    idt[irq_num].reserved1 = 1;
    idt[irq_num].reserved0 = 0;

    /* for system call or exception use trap config,
     otherwise for interrupt use interrupt config */

    if(irq_num >= 0x20 && irq_num <= 0x2F) {
        idt[irq_num].reserved3 = 0;
    } 

    //FOR EXCEPTIONS
    SET_IDT_ENTRY(idt[irq_num], handler_address);
}

/* setup_idt
 * 
 * Sets up the idt entries for system calls,
 * hardware interrupts, and exceptions.
 * Inputs: None
 * Outputs: None
 * Side Effects: None
 */

void setup_idt() { //replace with assembly linkage
    /* setup system call */
    setup_idt_entry(sys_call, 0x80, 3); 

    /* setup interrupts */
    setup_idt_entry(isr_wrapper20, 0x20, 0);
    setup_idt_entry(isr_wrapper21, 0x21, 0);
    setup_idt_entry(isr_wrapper22, 0x22, 0);
    setup_idt_entry(isr_wrapper23, 0x23, 0);
    setup_idt_entry(isr_wrapper24, 0x24, 0);
    setup_idt_entry(isr_wrapper25, 0x25, 0);
    setup_idt_entry(isr_wrapper26, 0x26, 0);
    setup_idt_entry(isr_wrapper27, 0x27, 0);
    setup_idt_entry(isr_wrapper28, 0x28, 0);
    setup_idt_entry(isr_wrapper29, 0x29, 0);
    setup_idt_entry(isr_wrapper2A, 0x2A, 0);
    setup_idt_entry(isr_wrapper2B, 0x2B, 0);
    setup_idt_entry(isr_wrapper2C, 0x2C, 0);
    setup_idt_entry(isr_wrapper2D, 0x2D, 0);
    setup_idt_entry(isr_wrapper2E, 0x2E, 0);
    setup_idt_entry(isr_wrapper2F, 0x2F, 0);

    //then initialize rtc, keyboard, cli sti

    /* setup exceptions */
    setup_idt_entry(divide, 0x00, 0);
    setup_idt_entry(debug, 0x01, 0);
    setup_idt_entry(nmi, 0x02, 0);
    setup_idt_entry(breakpoint, 0x03, 0);
    setup_idt_entry(overflow, 0x04, 0);
    setup_idt_entry(boundexceed, 0x05, 0);
    setup_idt_entry(invalidopcode, 0x06, 0);
    setup_idt_entry(decidenot, 0x07, 0);
    setup_idt_entry(doublefault, 0x08, 0);
    setup_idt_entry(invalidtss, 0x0A, 0);
    setup_idt_entry(segmentnot, 0x0B, 0);
    setup_idt_entry(stacksegfault, 0x0C, 0);
    setup_idt_entry(genprotfault, 0x0D, 0);
    setup_idt_entry(pagefault, 0x0E, 0);
    setup_idt_entry(floatingpoint, 0x10, 0);
    setup_idt_entry(alignment_c, 0x11, 0);
    setup_idt_entry(machine_c, 0x12, 0);
    setup_idt_entry(simd, 0x13, 0);
    setup_idt_entry(virtual_e, 0x14, 0);
    setup_idt_entry(security, 0x1E, 0);
}

/* system_call_handler
 * 
 * Prints that a system call was executed.
 * Inputs: None
 * Outputs: None
 * Side Effects: None
 */

void system_call_handler()
{
    printf("system call executed");
}


/* divide
 * 
 * Prints that a divide by zero exception occured.
 * Inputs: None
 * Outputs: None
 * Side Effects: None
 */

void divide() {
    cli();
    clear();
    printf("Divide-by-zero Error");
    while(1) ;
}

/* debug
 * 
 * Prints that a debug exception occured.
 * Inputs: None
 * Outputs: None
 * Side Effects: None
 */

void debug() {
    cli(); 
    clear();
    printf("Debug");
    while(1) ;
}

/* non-maskable interrupt
 * 
 * Prints that a non-maskable interrupt occured.
 * Inputs: None
 * Outputs: None
 * Side Effects: None
 */

void nmi() {
    cli(); 
    clear();
    printf("Non-maskable Interrupt");
    while(1) ;
}

/* breakpoint
 * 
 * Prints that a breakpoint exception occured.
 * Inputs: None
 * Outputs: None
 * Side Effects: None
 */

void breakpoint() {
    cli(); 
    clear();
    printf("Breakpoint");
    while(1) ;
}

/* overflow
 * 
 * Prints that a overflow exception occured.
 * Inputs: None
 * Outputs: None
 * Side Effects: None
 */

void overflow() {
    cli(); 
    clear();
    printf("Overflow");
    while(1) ;
}

/* boundexceed
 * 
 * Prints that the bound range was exceed.
 * Inputs: None
 * Outputs: None
 * Side Effects: None
 */

void boundexceed() {
    cli();
    clear();
    printf("Bound Range Exceeded");
    while(1) ;
}

/* invalidopcode
 * 
 * Prints that an invalid opcode exception occured.
 * Inputs: None
 * Outputs: None
 * Side Effects: None
 */
void invalidopcode() {
    cli(); 
    clear();
    printf("Invalid Opcode");
    while(1) ;
}


/* decidenot
 * 
 * Prints that a decide not available exception occured.
 * Inputs: None
 * Outputs: None
 * Side Effects: None
 */

void decidenot() {
    cli(); 
    clear();
    printf("Decide Not Available");
    while(1) ;
}

/* doublefault
 * 
 * Prints that a double fault exception occured.
 * Inputs: None
 * Outputs: None
 * Side Effects: None
 */

void doublefault() {
    cli(); 
    clear();
    printf("Double Fault");
    while(1) ;
}

/* invalidtss
 * 
 * Prints that the invalid tss was invalid.
 * Inputs: None
 * Outputs: None
 * Side Effects: None
 */

void invalidtss() {
    cli(); 
    clear();
    printf("Invalid TSS");
    while(1) ;
}

/* segmentnot
 * 
 * Prints that a segment is not present.
 * Inputs: None
 * Outputs: None
 * Side Effects: None
 */

void segmentnot() {
    cli(); 
    clear();
    printf("Segment Not Present");
    while(1) ;
}

/* stacksegfault
 * 
 * Prints that a stack segment fault occured.
 * Inputs: None
 * Outputs: None
 * Side Effects: None
 */

void stacksegfault() {
    cli(); 
    clear();
    printf("Stack-Segment Fault");
    while(1) ;
}

/* genprotfault
 * 
 * Prints that a general protection fault occured.
 * Inputs: None
 * Outputs: None
 * Side Effects: None
 */

void genprotfault() {
    cli(); 
    clear();
    printf("General Protection Fault");
    while(1) ;
}

/* pagefault
 * 
 * Prints that a page fault occured.
 * Inputs: None
 * Outputs: None
 * Side Effects: None
 */

void pagefault() {
    cli(); 
    //clear();
    printf("Page Fault");
    while (1)
    {
        //nothing
    }
    
}

/* floatingfault
 * 
 * Prints that a floating point exception occured.
 * Inputs: None
 * Outputs: None
 * Side Effects: None
 */

void floatingpoint() {
    cli(); 
    clear();
    printf("x87 Floating-Point-Exception");
    while(1) ;
}

/* alignment_c
 * 
 * Prints that an aligment check exception occured.
 * Inputs: None
 * Outputs: None
 * Side Effects: None
 */
void alignment_c() {
    cli(); 
    clear();
    printf("Alignment Check");
    while(1) ;
}

/* machine_c
 * 
 * Prints that a machine check exception occured.
 * Inputs: None
 * Outputs: None
 * Side Effects: None
 */

void machine_c() {
    cli(); 
    clear();
    printf("Machine Check");
    while(1) ;
}

/* simd
 * 
 * Prints that a simd exception occured.
 * Inputs: None
 * Outputs: None
 * Side Effects: None
 */

void simd() {
    cli(); 
    clear();
    printf("SIMD Floating-Point Exception");
    while(1) ;
}

/* virtual_e
 * 
 * Prints that a virtual exception occured.
 * Inputs: None
 * Outputs: None
 * Side Effects: None
 */

void virtual_e() {
    cli(); 
    clear();
    printf("Virtualization Exception");
    while(1) ;
}

/* security
 * 
 * Prints that a security exception occured.
 * Inputs: None
 * Outputs: None
 * Side Effects: None
 */

void security() {
    cli(); 
    clear();
    printf("Security Exception");
    while(1) ;
}
