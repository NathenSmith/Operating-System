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
//ints
extern void rtc_handler(void);
extern void keyboard_handler(void);

void rtc_handler(void){
    //check for hardware ints
    
}
void keyboard_handler(void){
    //check for hardware ints
    
}


void setup_idt_entry(void * handler_address, int irq_num, int privilege)
{   
     //FOR EXCEPTIONS
    SET_IDT_ENTRY(idt[irq_num], handler_address);

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
}

/* initializes all idt entries */
void setup_idt() { //replace with assembly linkage
    /* setup system call */
    setup_idt_entry(sys_call, 0x80, 3); 

    /* setup interrupts */
    setup_idt_entry(isr_wrapper0, 0x20, 0);
    setup_idt_entry(isr_wrapper1, 0x21, 0);
    setup_idt_entry(isr_wrapper2, 0x22, 0);
    setup_idt_entry(isr_wrapper3, 0x23, 0);
    setup_idt_entry(isr_wrapper4, 0x24, 0);
    setup_idt_entry(isr_wrapper5, 0x25, 0);
    setup_idt_entry(isr_wrapper6, 0x26, 0);
    setup_idt_entry(isr_wrapper7, 0x27, 0);
    setup_idt_entry(isr_wrapper8, 0x28, 0);
    setup_idt_entry(isr_wrapper9, 0x29, 0);
    setup_idt_entry(isr_wrapperA, 0x2A, 0);
    setup_idt_entry(isr_wrapperB, 0x2B, 0);
    setup_idt_entry(isr_wrapperC, 0x2C, 0);
    setup_idt_entry(isr_wrapperD, 0x2D, 0);
    setup_idt_entry(isr_wrapperE, 0x2E, 0);
    setup_idt_entry(isr_wrapperF, 0x2F, 0);

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

void system_call_handler()
{
    printf("system call executed");
}

void divide() {
    printf("Divide-by-zero Error");
    while(1) {
        //do nothing
    }
}
void debug() {
    printf("Debug");
    while(1) {
        //do nothing
    }
}
void nmi() {
    printf("Non-maskable Interrupt");
    while(1) {
        //do nothing
    }
}
void breakpoint() {
    printf("Breakpoint");
    while(1) {
        //do nothing
    }
}
void overflow() {
    printf("Overflow");
    while(1) {
        //do nothing
    }
}
void boundexceed() {
    printf("Bound Range Exceeded");
    while(1) {
        //do nothing
    }
}
void invalidopcode() {
    printf("Invalid Opcode");
    while(1) {
        //do nothing
    }
}

void decidenot() {
    printf("Decide Not Available");
    while(1) {
        //do nothing
    }
}
void doublefault() {
    printf("Double Fault");
    while(1) {
        //do nothing
    }
}
void invalidtss() {
    printf("Invalid TSS");
    while(1) {
        //do nothing
    }
}
void segmentnot() {
    printf("Segment Not Present");
    while(1) {
        //do nothing
    }
}
void stacksegfault() {
    printf("Stack-Segment Fault");
    while(1) {
        //do nothing
    }
}
    
void genprotfault() {
    printf("General Protection Fault");
    while(1) {
        //do nothing
    }
}
void pagefault() {
    printf("Page Fault");
    while(1) {
        //do nothing
    }
}
void floatingpoint() {
    printf("x87 Floating-Point-Exception");
    while(1) {
        //do nothing
    }
}


void alignment_c() {
    printf("Alignment Check");
    while(1) {
        //do nothing
    }
}
void machine_c() {
    printf("Machine Check");
    while(1) {
        //do nothing
    }
}
void simd() {
    printf("SIMD Floating-Point Exception");
    while(1) {
        //do nothing
    }
}
void virtual_e() {
    printf("Virtualization Exception");
    while(1) {
        //do nothing
    }
}
void security() {
    printf("Security Exception");
    while(1) {
        //do nothing
    }
}
