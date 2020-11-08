#include "filesystem.h"

uint32_t filesystem_start_addr;    //starting address of filesystem
uint32_t datablocks_start_address; //address of first datablock

boot_block_t boot_block_original; //boot block variable and pointer
boot_block_t * boot_block = &boot_block_original;

inode_t inodes_original; //inodes variable and pointer
inode_t * inodes = &inodes_original;

dentry_t curr_file_original;
dentry_t * curr_file = &curr_file_original; //contains the current file

uint32_t currentDirectoryEntry;

/*  init_filesystem
    initialzies several variables necessary for utilizing the file system
    Inputs: start_addr - starting address of file system
    Outputs: none
    Return Value: none
*/

void init_filesystem(uint32_t start_addr){
  filesystem_start_addr = start_addr; //
  boot_block = (boot_block_t *)filesystem_start_addr;
  inodes = (inode_t *)(filesystem_start_addr + BLOCK_SIZE);
  uint32_t n_inodes = boot_block->inode_count;
  datablocks_start_address = filesystem_start_addr + BLOCK_SIZE + BLOCK_SIZE*n_inodes;
  currentDirectoryEntry = 0;
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
    uint32_t i;

    for(i = 0; i < boot_block->dir_count; i++) {
        if(strncmp((boot_block->direntries)[i].filename, (int8_t *)fname, FILENAME_LEN) == 0){
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
    strncpy(dentry->filename, entry.filename, FILENAME_LEN);
    dentry->filetype = entry.filetype;
    dentry->inode_num = entry.inode_num;
    strncpy(dentry->reserved, entry.reserved, RESERVED_LENGTH_DENTRY);

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
    if(inode_number < 0 || inode_number > boot_block->inode_count) return -1;
    inode_t currentInode = inodes[inode_number];

    uint32_t offsetBlocks = offset / BLOCK_SIZE; //calculate which block to start at
    uint32_t offsetBlocksRemainder = offset % BLOCK_SIZE; //calculate where in block to start

    //validate offsetBlocks aka blockNumber
    if(offsetBlocks < 0 || offsetBlocks > boot_block->data_count) return -1;

    uint32_t start_addr_to_copy = datablocks_start_address +
     (currentInode.data_block_num[offsetBlocks])*BLOCK_SIZE +
     offsetBlocksRemainder;

    //find length of data to copy
    //min() is to make sure it only reads up to the length of the file.
    uint32_t position_after_copy = min(currentInode.length, offset + length);
    uint32_t length_to_copy = position_after_copy - offset;

    /* If length to copy is small enough that it does not require going
     onto the next block, do a simple copy of length length_to_copy. */

    if((offset + length_to_copy)/BLOCK_SIZE == offsetBlocks) { // if it would still be in the same block #
        memcpy(buf, start_addr_to_copy, length_to_copy);
    }
    /* Otherwise, we have to copy over multiple blocks. */
    else {
        //copy first block data
        uint32_t n_bytes_left_in_block = BLOCK_SIZE - offsetBlocksRemainder;
        memcpy(buf, start_addr_to_copy, n_bytes_left_in_block);        
        buf += n_bytes_left_in_block;

        //now we loop through the other blocks
        int i = offsetBlocks + 1;
        int nBytesRead = n_bytes_left_in_block;

        while(nBytesRead < length_to_copy) {
            start_addr_to_copy = datablocks_start_address + (currentInode.data_block_num[i])*BLOCK_SIZE;
            uint32_t length_left_to_copy = length_to_copy - nBytesRead;
            //if we're in the final block, copy the remaining data.
            if(length_left_to_copy <= BLOCK_SIZE) { 
                memcpy(buf, start_addr_to_copy, length_left_to_copy);
                buf += length_left_to_copy;
                nBytesRead += length_left_to_copy;
            }
            //otherwise, copy a 4kB block of data.
            else {
                memcpy(buf, start_addr_to_copy, BLOCK_SIZE);
                buf += BLOCK_SIZE;
                nBytesRead += BLOCK_SIZE;
            }
            i++;
        }

    }

    return length_to_copy;
}

/* min
    returns the smaller number between a and b

    Inputs: a, b are integers
    Outputs: none
    Return Value: if a < b, return a. Otherwise return b.
    Side Effects: none
*/

int32_t min(uint32_t a, uint32_t b) {
    if(a < b) {
        return a;
    }
    else {
        return b;
    }
}

/* file_read
    calls read_data for a file, reading a given amount of bytes
    Inputs:
      fd - file descriptor, not used in the function
      buf - buffer to write file data into
      nbytes - number of bytes to write into buffer
    Outputs:
        uint8_t * buf - the pointer to the buffer in which the data needs to be stored
    Return Value: returns the number of bytes read. If unable to read, it returns -1.
    Side Effects: none
*/
int32_t file_read(int32_t fd, uint8_t * buf, int32_t nbytes) {
    
    uint32_t nBytesRead = read_data(curr_file->inode_num, child_pcb->file_arr[fd].file_pos, buf, nbytes);
    if(nBytesRead == -1) {
        return -1;
    }
    else {
        child_pcb->file_arr[fd].file_pos += nBytesRead; // keep track of number of bytes read
        return nBytesRead;
    }
}

/* file_write
    file system is read only
    Inputs:
      fd - file descriptor, not used in the function
      buf - buffer with data to write
      nbytes - number of bytes to write
    Outputs: None
    Return Value: file system is read only so return -1
    Side Effects: none
*/
int32_t file_write(int32_t fd, uint8_t * buf, int32_t nbytes){
    return -1;
}

/* file_open
    given a filename, it initialzies variables and marks file in as in use
    Inputs:
      filename - name of file
    Outputs: None
    Return Value: return -1 if cannot find file, 0 otherwise
    Side Effects: none
*/
int32_t file_open(const uint8_t* filename) {
    int status = read_dentry_by_name (filename, curr_file);

    if(status == -1) { // file does not exist
        return -1;
    }else {
        return 0;
    }
}

/* file_close
    mark file as not in use
    Inputs:
      fd - file descriptor
    Outputs: None
    Return Value: return 0 successfully closed file, -1 otherwise
    Side Effects: none
*/

int32_t file_close(int32_t fd){
    //the user is not allowed to close the default files
    if(fd == 0 || fd == 1) {
        return -1;
    }
    file_in_use = 0; // mark file as no longer in use
    return 0;
}

/* dir_read
    returns file names present in directory
    Inputs:
      fd - file descriptor, not used in the function
      buf - buffer to write file data into
      nbytes - number of bytes to write into buffer
    Outputs:
        uint8_t * buf - the pointer to the buffer in which the data needs to be stored
    Return Value: returns the number of bytes read. If unable to read, it returns -1.
    Side Effects: none
*/

int32_t dir_read(int32_t fd, void* buf, int32_t nbytes) {
    if(currentDirectoryEntry < boot_block->dir_count) {
        strcpy(buf, boot_block->direntries[currentDirectoryEntry].filename); 
        currentDirectoryEntry++;
    }
    return 0;
}

/* dir_write
    file system is read only
    Inputs:
      fd - file descriptor, not used in the function
      buf - buffer with data to write
      nbytes - number of bytes to write
    Outputs: None
    Return Value: file system is read only so return -1
    Side Effects: none
*/
int32_t dir_write(int32_t fd, void* buf, int32_t nbytes){
    return -1;
}

/* dir_open
    given a filename, it initialzies variables and marks file in as in use
    Inputs:
      filename - name of file
    Outputs: None
    Return Value: return -1 if directory does not exist, 0 otherwise
    Side Effects: none
*/
int32_t dir_open(const uint8_t* filename) {
    if(file_in_use) {
        return -1;
    }

    int status = read_dentry_by_name (filename, curr_file);

    if(status == -1) { // directory does not exist
        return -1;
    }
    else {
        file_in_use = 1; // mark as in use
        return 0;
    }
}

/* dir_close
    mark file as not in use
    Inputs:
      fd - file descriptor
    Outputs: None
    Return Value: return 0 successfully closed file, -1 otherwise
    Side Effects: none
*/
int32_t dir_close(int32_t fd){
    //the user is not allowed to close the default files
    if(fd == 0 || fd == 1) {
        return -1;
    }
    file_in_use = 0;
    return 0;
}
