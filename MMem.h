#ifndef MMEM_H
#define MMEM_H
/* Main Memory simulation */
#include <stdbool.h>
#include <stdio.h>

/*sizes in bytes*/

#define PAGE_SIZE 4096
#define OFFSET_SIZE 12 /*log(PAGE_SIZE)*/

#define PID_BZIP 1
#define PID_GCC 2

void mem_initialize(int nframes, char* alg); /*initialize main memory with given number of frames. Frames are empty*/
void mem_insert(int page_number, int pid, char rw);  /*insert a page into main memory using repalcement algorithm*/
void mem_delete(); /*delete simulator*/
void mem_print(); /*print current frames of memory*/

void const print_stats(); /*print reads, writes and pagefaults*/

#endif
