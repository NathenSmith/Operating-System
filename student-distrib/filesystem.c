#include "filesystem.h"

//files_t* files;
u_int32_t filesystem_start_addr;
data_block_t data_blocks[];
boot_block_t* boot_block;
inode_t inodes[N_INODES];


/*  init_filesystem
    Inputs:
    Outputs:
    Return Value:
*/

void init_filesystem(u_int32_t start_addr){
  filesystem_start_addr = start_addr;
  boot_block = filesystem_start_addr;
  inodes = filesystem_start_addr + BLOCK_SIZE;
}

/* read_dentry_by_name
    reads a directory entry by name and populates the passed in
    dentry with the corresponding filename, file type, and inode #.

    Inputs:
        const uint8_t * fname - specifies the name of the directory entry to read
        MUST BE ZERO PADDED
    Outputs:
        dentry_t* dentry - the pointer to the dentry structure in which
        the data needs to be populated
    Return Value: returns 0 if reading was successful, otherwise returns -1.
    Side Effects: none
*/

int32_t read_dentry_by_name (const uint8_t * fname, dentry_t* dentry) {
    /* scans through dir entries in boot block to find file name, find corresponding index */

    int found = 0;
    uint32_t i;

    for(i = 0; i < boot_block->dir_count; i++) {
        if(strncmp(boot_block->direntries[i]->filename, fname, 32) == 0){
          /* read directory entry by index corresponding to name */
          return read_dentry_by_index(i, dentry);
        }
    }

    return -1;
}

/* read_dentry_by_index
    reads a directory entry by index and populates the passed in
    dentry with the corresponding filename, file type, and inode #.

    Inputs:
        const uint32_t index - specifies the index of the directory entry to read
    Outputs:
        dentry_t* dentry - the pointer to the dentry structure in which
        the data needs to be populated
    Return Value: returns 0 if reading was successful, otherwise returns -1.
    Side Effects: none
*/

int32_t read_dentry_by_index (uint32_t index, dentry_t * dentry) {
    dentry_t entry;
    entry = boot_block->direntries[index];

    if(index < 0 || index >= boot_block->dir_count) return -1;

    /* Deepcopy the entry at that index into the passed in dentry structure. */
    strcpy(dentry->filename, entry.filename);
    dentry->filetype = entry.filetype;
    dentry->inode_num = entry.inode_num;
    strcpy(dentry->reserved, entry.reserved);

    return dentry;
}

/* read_data
    reads length bytes from a file corresponding to the inode number inode.
    Inputs:
        const uint32_t inode_number - specifies the inode number corresponding to the
        file to read
        uint32_t offset - specifies the offset from the start of the file to
        start reading data
        uint32_t length - specifies the number of bytes to be read.

    Outputs:
        uint8_t * buf - the pointer to the buffer in which
        the data needs to be stored
    Return Value: returns the number of bytes read if it hasn't reached the end
    of the file, otherwise returns 0. If unable to read, it returns -1.
    Side Effects: none
*/

int32_t read_data (uint32_t inode_number, uint32_t offset, uint8_t * buf, uint32_t length) {
    //validate inode
    if(inode_number >= 0 && inode_number < boot_block->inode_count) return -1;
    inode_t currentInode = inodes[inode_number];

    u_int32_t offsetBlocks = offset / BLOCK_SIZE;

    //validate offsetBlocks aka blockNumber
    if(offsetBlocks >= 0 && offsetBlocks < boot_block->data_count) return -1;

    u_int32_t offsetBlocksRemainder = offset % BLOCK_SIZE;
    u_uint32_t start_addr_in_datablock = currentInode->data_block_num[offsetBlocks] + offsetBlocksRemainder;

    // for(int i = 0; i < length; i++){
    //   buf[i] = *(start_addr_in_datablock + i);
    // }

    strncpy(buf[i], start_addr_in_datablock + i, length);

    return 0;
}

/* filesystem_read */
int32_t filesystem_read(int32_t fd, void* buf, int32_t nbytes){

}

/* filesystem_write */
int32_t filesystem_write(int32_t fd, void* buf, int32_t nbytes){

}

/* filesystem_open */
int32_t filesystem_open(const uint8_t* filename){

}

/* filesystem_close */
int32_t filesystem_close(int32_t fd){

}
