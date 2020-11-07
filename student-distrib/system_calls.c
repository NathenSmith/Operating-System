#include "system_calls.h"
#include "lib.h"
#include "execute.h"
#include "filesystem.h"
#include "rtc.h"

typedef struct
{
    int32_t (*read)(int32_t fd, void* buf, int32_t nbytes);
    int32_t (*write)(int32_t fd, const int32_t* buf, int32_t nbytes);
    int32_t (*open)(const uint8_t* filename);
    int32_t (*close)(int32_t fd);
}  rtc_ptrs;

typedef struct
{
    int32_t (*read)(int32_t fd, void* buf, int32_t nbytes);
    int32_t (*write)(int32_t fd, const int32_t* buf, int32_t nbytes);
    int32_t (*open)(const uint8_t* filename);
    int32_t (*close)(int32_t fd);
} file_ptrs;

typedef struct
{
    int32_t (*read)(int32_t fd, void* buf, int32_t nbytes);
    int32_t (*write)(int32_t fd, const int32_t* buf, int32_t nbytes);
    int32_t (*open)(const uint8_t* filename);
    int32_t (*close)(int32_t fd);
} dir_ptrs;




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

int32_t read(int32_t fd,void* buf, int32_t nbytes) {
    if(fd < 0 || fd >= 8 || !(file_desc_array[fd].flag)){ //not in bounds or not open
        return -1;
    }
    return pcb->file_desc_array[fd].file_op_ptr->read(fd, buf, nbytes);
}

/* write
 * 
 * Implements the write system call.
 * Inputs: None
 * Outputs: None
 * Side Effects: None
 */

int32_t write(int32_t fd, void* buf, int32_t nbytes) {
    if(fd < 0 || fd >= 8 || !(file_desc_array[fd].flag)){ //not in bounds or not open
        return -1;
    }
    return pcb->file_desc_array[fd].file_op_ptr->write(fd, buf, nbytes);
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
    dentry_t file_dentry;
    if(read_dentry_by_name (filename, &file_dentry) == -1) return -1;
    int i;
    for(i=2; i< 8; i++){
        if(!(pcb->file_desc_array[i].flag)){
            pcb->file_desc_array[i].flag = 1;
            //if statements, go through each type of device
            //make fileop table point to respective table
            switch (file_dentry.filetype)
            {
            case 0: //real-time clock
                pcb->file_desc_array[i].inode_num = 0; //should be ignored here and directory?
                //file pos?
                pcb->file_desc_array[i].file_pos = n_bytes_read_so_far[i]; 
                rtc_ptrs rtc_ptr = {rtc_read, rtc_write, rtc_open, rtc_close};
                pcb->file_desc_array[i].file_op_ptr = &rtc_ptr;
                rtc_ptr.open(filename);
                break;
            case 1: //directory
                /* code */
                pcb->file_desc_array[i].inode_num = 0; //should be ignored here and directory?
                //file pos?
                pcb->file_desc_array[i].file_pos = n_bytes_read_so_far[i];
                dir_ptrs dir_ptr = {dir_read, dir_write, dir_open, dir_close};
                pcb->file_desc_array[i].file_op_ptr = &dir_ptr;
                dir_ptr.open(filename);
                break;
            case 2: //regular file
                /* code */
                pcb->file_desc_array[i].inode_num = file_dentry.inode_num; //should be ignored here and directory?
                //file pos?
                pcb->file_desc_array[i].file_pos = n_bytes_read_so_far[i];
                file_ptrs file_ptr = {file_read, file_write, file_open, file_close};
                pcb->file_desc_array[i].file_op_ptr = &file_ptr;
                file_ptr.open(filename);
                break;
            default: //should never reach here
                i = -1;
                break;
            }
            return i;
        }
        
    }
    return -1;
}

/* close
 * 
 * Implements the close system call.
 * Inputs: None
 * Outputs: None
 * Side Effects: None
 */

int32_t close(int32_t fd) {
    //error check fd
    if(fd < 2 || fd >= 8 || !(file_desc_array[fd].flag)){ //not in bounds or not open
        return -1;
    }
    pcb->file_desc_array[fd].flag = 0;
    pcb->file_desc_array[fd].inode_num = 0; //should be ignored here and directory?
    //file pos?
    pcb->file_desc_array[fd].file_pos = 0;
    pcb->file_desc_array[i].file_op_ptr = 0;
    return 0;
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
