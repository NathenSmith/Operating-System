#include "filesystem.h"

uint32_t filesystem_start_addr;
boot_block_t * boot_block;
inode_t * inodes;
file_desc_t * pcb;
file_op_t * file_operations;
dir_op_t * directory_operations;
rtc_op_t * rtc_operations;

/*  init_filesystem
    Inputs:
    Outputs:
    Return Value:
*/

void init_filesystem(uint32_t start_addr){
  filesystem_start_addr = start_addr;
  boot_block = filesystem_start_addr;
  inodes = filesystem_start_addr + BLOCK_SIZE;
  int i;

  //initialize file descriptors for stdin and stdout
  pcb[0].flags = 1;
  pcb[1].flags = 1;

  //initialize all other file descriptors to 0
  for(i = 2; i < 8; i++) {
      pcb[i].flags = 0;
  }
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

    return 0;
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
    Return Value: returns the number of bytes read. If unable to read, it returns -1.
    Side Effects: none
*/

int32_t read_data (uint32_t inode_number, uint32_t offset, uint8_t * buf, uint32_t length) {

    /* offset represents the number of bytes that have been read so far. */



    //validate inode
    if(inode_number >= 0 && inode_number < boot_block->inode_count) return -1;
    inode_t currentInode = inodes[inode_number];

    uint32_t offsetBlocks = offset / BLOCK_SIZE;

    //validate offsetBlocks aka blockNumber
    if(offsetBlocks >= 0 && offsetBlocks < boot_block->data_count) return -1;

    uint32_t offsetBlocksRemainder = offset % BLOCK_SIZE;
    uint32_t start_addr_in_datablock = currentInode.data_block_num[offsetBlocks] + offsetBlocksRemainder;

    //find length of data to copy
    uint32_t position_after_copy = min(currentInode.length, offset + length);
    uint32_t length_to_copy = position_after_copy - offset;
    
    /* If length to copy is small enough that it does not require going
     onto the next block, do a simple copy of length length_to_copy. */
    if((offset + length_to_copy)/BLOCK_SIZE == offsetBlocks) {
        strncpy(buf, start_addr_in_datablock, length_to_copy);
    }
    /* Otherwise, we have to copy over multiple blocks. */
    else {
        int block;
        int final_block = position_after_copy/BLOCK_SIZE;
        int final_in_block_offset = position_after_copy % BLOCK_SIZE;
        for(block = offsetBlocks; block <= final_block; block++) {
            //first block 
            if(block == offsetBlocks) {
                //addr of end of block - start addr in block
                uint32_t block_length_to_copy = currentInode.data_block_num[offsetBlocks] 
                                                + BLOCK_SIZE - start_addr_in_datablock;
                strncpy(buf, start_addr_in_datablock, block_length_to_copy);
            }
            //last block
            else if(block == final_block) {
                strncpy(buf, currentInode.data_block_num[block], final_in_block_offset);
            }
            //all blocks in between first and last block
            else {
                strncpy(buf, currentInode.data_block_num[block], BLOCK_SIZE);
            }
        }
    }
    return length_to_copy;
}

int32_t min(uint32_t a, uint32_t b) {
    if(a < b) {
        return a;
    }
    else {
        return b;
    }
}

/* file read */
int32_t file_read(int32_t fd, void* buf, int32_t nbytes) {
    uint32_t nBytesRead = read_data(pcb[fd].inode_index, pcb[fd].file_position, buf, nbytes);
    if(nBytesRead == -1) {
        return -1;
    }
    else {
        pcb[fd].file_position += nBytesRead;
        return nBytesRead;
    }
}

/* file write */
int32_t file_write(int32_t fd, void* buf, int32_t nbytes){
    return -1;
}

/* file open */
int32_t file_open(const uint8_t* filename){
    dentry_t entry;
    int status;
    status = read_dentry_by_name (filename, &entry);
    
    //return -1 if file does not exist
    if(status == -1) {
        return -1;
    }

    //allocate unused file_descriptor
    int i;
    for(i = 2; i < 8; i++) {
        if(pcb[i].flags == 0) {
            pcb[i].flags = 1;
            if(entry.filetype == 0) {
                pcb[i].file_op = rtc_operations;
            }
            if(entry.filetype == 1) {
                pcb[i].file_op = directory_operations;
                pcb[i].file_position = 0;
            }
            else if(entry.filetype == 2) {
                pcb[i].file_op = file_operations;
            }
            return 0;
        }
    }

    //return -1 if all file descriptors are in use(reaches end of loop)
    return -1;
}

/* file_close */
int32_t file_close(int32_t fd){
    //the user is not allowed to close the default files
    if(fd == 0 || fd == 1) {
        return -1;
    }
    pcb[fd].flags = 0;
}

/* dir read */
int32_t dir_read(int32_t fd, void* buf, int32_t nbytes){

}

/* dir write */
int32_t dir_write(int32_t fd, void* buf, int32_t nbytes){
    return -1;
}

/* dir open */
int32_t dir_open(const uint8_t* filename){

}

/* dir_close */
int32_t dir_close(int32_t fd){

}

