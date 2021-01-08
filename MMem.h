#ifndef MMEM_H
#define MMEM_H

#include <stdbool.h>
#include "HashedPT.h"


void MMem_initialize(int nframes); /*initialize main memory with given number of frames. Frames are empty*/
bool MMem_isHit(int data); 
int MMem_getHitIndex(int data);
void MMem_dispPages();
void MMem_delete();

// void lru(char* ref, int* least);

int findLRU();
void MMem_insert(char* ref, HashedPT pt);

#endif