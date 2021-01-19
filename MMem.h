#ifndef MMEM_H
#define MMEM_H

#include <stdbool.h>
#include <stdio.h>

/*sizes in bytes*/

#define PAGE_SIZE 4096
#define OFFSET_SIZE 12 /*log(PAGE_SIZE)*/

#define PID_BZIP 1
#define PID_GCC 2

void mem_initialize(int nframes, char* alg); /*initialize main memory with given number of frames. Frames are empty*/
void mem_insert(int page_number, int pid, char rw); 
void mem_delete();
void mem_print();

void const print_stats();

#endif
