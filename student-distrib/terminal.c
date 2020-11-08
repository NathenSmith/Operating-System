#include "terminal.h"
#include "lib.h"

/* terminal_read
 * 
 * Description: Copies from keyboard buffer to terminal buffer the appropriate characters while maintaining the limit.
 * Inputs: Terminal buffer
 * Outputs: none
 * Side Effects: Changes terminal buffer
 * Return value: Number of bytes read
 */ 
uint32_t terminal_read (uint32_t fd, void* buf, uint32_t nbytes){
	if(buf == NULL || nbytes <= 0) return 0;
	//clear the buffer being sent in

	char * buf_ = (char *)buf;

	int i;
	for(i = 0; i < BUF_SIZE; i++){
		buf_[i] = '\0';
	}
	terminal_flag = 0;
	while(!terminal_flag){
		//wait for newline to be entered
	}
	uint32_t j = 0;
	while(j != BUF_SIZE){
		//copy from keyboard buf to terminal buf the appropriate characters
		if(j == nbytes) break; //reached the maximum bytes acceptable
		if(kbd_buf[j] == '\0') continue; //return to beginning of loop		
		buf_[j] = kbd_buf[j];
		if(buf_[j] == '\n'){
			j++;
			break;
		}
		j++; //not newline
		
	}
	terminal_flag = 0; //resets the flag to accept another newline
	return j; //returns number of bytes
}

/* terminal_write
 * 
 * Description: Writes from keyboard buf to screen
 * Inputs: Terminal buffer, number of bytes to write
 * Outputs: none
 * Side Effects: Writes to screen
 * Return value: number of bytes written
 */ 
uint32_t terminal_write (uint32_t fd, const void* buf, uint32_t nbytes){
	if(buf == NULL || nbytes <= 0) return -1; //no bytes to read
	uint32_t i, counter = 0;

	char * buf_ = (char *)buf;

	for(i = 0; i < nbytes; i++){
		if(i == BUF_SIZE) break; //iterates until reaches max size of buffer or the number of bytes
		if(buf_[i] != '\0'){ //will ignore NULL
			putc(buf_[i]); //write to screen
			counter++;
		} 
	}
	for(i = 0; i < BUF_SIZE; i++){
		kbd_buf[i] = '\0'; //reset keyboard buf
	}
	
    return counter; //number of bytes read
}
/* terminal_open
 * 
 * Description: Opens terminal
 * Inputs: None used
 * Outputs: none
 * Side Effects: none
 * Return value: Zero on success
 */ 
uint32_t terminal_open (const uint8_t* filename){
    return 0;
}
/* terminal_close
 * 
 * Description: Closes terminal
 * Inputs: None used
 * Outputs: none
 * Side Effects: None
 * Return value: Zero on success
 */ 
uint32_t terminal_close (uint32_t fd){
    return -1;
}
