#include "filesystem.h"

files_t files;

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
    uint8_t index = 0;         
    while(strncmp(files.boot_block.direntries[index].filename, fname, 32) != 0) {
        index++;
    }
    /* read directory entry by index corresponding to name */
    read_dentry_by_index(index, dentry);
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
    entry = files.boot_block.direntries[index];

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
        const uint32_t inode - specifies the inode number corresponding to the
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

int32_t read_data (uint32_t inode, uint32_t offset, uint8_t * buf, uint32_t length) {
    length = 
}