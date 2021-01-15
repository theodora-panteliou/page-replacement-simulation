#ifndef MMEM_H
#define MMEM_H

#include <stdbool.h>
#include <stdio.h>
#include "HashedPT.h"

typedef struct mem_entry{
    int page_number;
    int pid;
} mem_entry;

bool *reference_bit;

void mem_initialize(int nframes, char* alg); /*initialize main memory with given number of frames. Frames are empty*/
void mem_delete();
void mem_insert(int page_number, int* frame, mem_entry* victim_page, int pid);
void mem_print();

#endif
