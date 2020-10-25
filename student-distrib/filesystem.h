#include "lib.h"

#define FILENAME_LEN 32
#define N_INODES 32
#define BLOCK_SIZE 4096
#define N_BLOCKS 32

int32_t read_dentry_by_name (const uint8_t * fname, dentry_t* dentry);
int32_t read_dentry_by_index (uint32_t index, dentry_t* dentry);
int32_t read_data (uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length);
void init();

typedef struct dentry
{
    int8_t filename[FILENAME_LEN];
    int32_t filetype;
    int32_t inode_num;
    int8_t reserved[24];
} dentry_t;

typedef struct boot_block
{
    int32_t dir_count;
    int32_t inode_count;
    int32_t data_count;
    int8_t reserved[52];
    dentry_t direntries[63];
} boot_block_t;

typedef struct inode
{
    int32_t length;
    int32_t data_block_num [1023];
} inode_t;

typedef struct file_descriptor 
{
    file_op_t * file_op;
    uint32_t inode_index; 
    uint32_t file_position;
    uint32_t flags; //0 means available, 1 means in use
} file_desc_t;

//this is probably wrong syntax, fix

typedef struct file_operations
{
    void * file_open;
    void * file_close;
    void * file_read;
    void * file_write;
} file_op_t;

typedef struct directory_operations
{
    void * dir_open;
    void * dir_close;
    void * dir_read;
    void * dir_write;
} dir_op_t;

typedef struct RTC_operations
{
    void * rtc_open;
    void * rtc_close;
    void * rtc_read;
    void * rtc_write;
} rtc_op_t;

typedef struct terminal_operations
{

} terminal_op_t;