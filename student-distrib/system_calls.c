#include "system_calls.h"
#include "lib.h"

/* halt
 * 
 * Implements the halt system call.
 * Inputs: None
 * Outputs: None
 * Side Effects: None
 */

void halt() {
    printf("halt");
    while(1) {}
} 

/* execute
 * 
 * Implements the execute system call.
 * Inputs: None
 * Outputs: None
 * Side Effects: None
 */

void execute() {
    printf("execute");
    while(1) {}
}

/* read
 * 
 * Implements the read system call.
 * Inputs: None
 * Outputs: None
 * Side Effects: None
 */

int32_t read(int32_t fd, void* buf, int32_t nbytes) {
    printf("read");
    while(1) {}
}

/* write
 * 
 * Implements the write system call.
 * Inputs: None
 * Outputs: None
 * Side Effects: None
 */

int32_t write(int32_t fd, void* buf, int32_t nbytes) {
    printf("write");
    while(1) {}
}

/* open
 * 
 * Implements the open system call.
 * Inputs: None
 * Outputs: None
 * Side Effects: None
 */

int32_t open(const uint8_t* filename) {
    printf("open");
    //iterate through pcb starting at index 2
    
    while(1) {}
}

/* close
 * 
 * Implements the close system call.
 * Inputs: None
 * Outputs: None
 * Side Effects: None
 */

int32_t close(int32_t fd) {
    printf("close");
    while(1) {}
}

/* getargs
 * 
 * Implements the getargs system call.
 * Inputs: None
 * Outputs: None
 * Side Effects: None
 */

int32_t getargs(uint8_t* buf, int32_t nbytes) {
    printf("getargs");
    while(1) {}
}

/* vidmap
 * 
 * Implements the vidmap system call.
 * Inputs: None
 * Outputs: None
 * Side Effects: None
 */

int32_t vidmap(uint8_t** screen_start) {
    printf("vidmap");
    while(1) {}
}

/* set_handler
 * 
 * Implements the set_handler system call.
 * Inputs: None
 * Outputs: None
 * Side Effects: None
 */

void set_handler() {
    printf("set_handler");
    while(1) {}
}

/* sigreturn
 * 
 * Implements the sigreturn system call.
 * Inputs: None
 * Outputs: None
 * Side Effects: None
 */

void sigreturn() {
    printf("sigreturn");
    while(1) {}
}
