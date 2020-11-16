#ifndef _FILESYSTEM_H
#define _FILESYSTEM_H
#include "lib.h"
#include "shared_global_variables.h"

#define FILENAME_LEN 32
#define BLOCK_SIZE 4096
#define N_DATA_BLOCKS 1023
#define RESERVED_LENGTH_DENTRY 24
#define RESERVED_LENGTH_BOOT_BLOCK 52
#define N_DIR_ENTRIES 63

typedef struct dentry
{
    uint8_t filename[FILENAME_LEN];
    uint32_t filetype;
    uint32_t inode_num;
    uint8_t reserved[RESERVED_LENGTH_DENTRY];
} dentry_t;

typedef struct boot_block
{
    uint32_t dir_count;
    uint32_t inode_count;
    uint32_t data_count;
    uint8_t reserved[RESERVED_LENGTH_BOOT_BLOCK];
    dentry_t direntries[N_DIR_ENTRIES];
} boot_block_t;

typedef struct inode
{
    uint32_t length;
    uint32_t data_block_num [N_DATA_BLOCKS];
} inode_t;

int32_t read_dentry_by_name (const uint8_t * fname, dentry_t* dentry);
int32_t read_dentry_by_index (uint32_t index, dentry_t* dentry);
int32_t read_data (uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length);
void init_filesystem(uint32_t start_addr);
int32_t min(uint32_t a, uint32_t b);
extern int32_t file_read(int32_t fd, void * buf, int32_t nbytes);
extern int32_t file_write(int32_t fd, const void * buf, int32_t nbytes);
extern int32_t file_open(const uint8_t* filename);
extern int32_t file_close(int32_t fd);
extern int32_t dir_read(int32_t fd, void* buf, int32_t nbytes);
extern int32_t dir_write(int32_t fd, const void* buf, int32_t nbytes);
extern int32_t dir_open(const uint8_t* filename);
extern int32_t dir_close(int32_t fd);
#endif
