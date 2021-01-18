#include "MMem.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int nframes;
static mem_entry* mmframe = NULL;
unsigned int timecounter = 0;
unsigned int* time;
static int (*replace_alg)();
static int pointer=0;

int findLRU();
int secondchance();

void mem_initialize(int nframesk,char* alg) {
    nframes = nframesk;

    mmframe = malloc(nframes*sizeof(mem_entry));
    reference_bit = malloc(nframes*sizeof(bool)); /*for 2nd chance*/
    time = malloc(nframes*sizeof(int)); /*for lru*/
    if (strcmp(alg, "LRU") == 0){
        replace_alg = findLRU;
    }
    if (strcmp(alg, "2C") == 0){
        replace_alg = secondchance;
    }
    
    timecounter = 0;
    for(int i=0; i<nframes; i++) {
        mmframe[i].page_number=-1;
        mmframe[i].pid=-1;
        time[i] = 0; /*for lru*/
        reference_bit[i] = true; /*for 2nd chance*/
    }
}

void mem_delete(){
    free(mmframe);
    mmframe = NULL;

    free(time); 
    time = NULL;

    free(reference_bit);
    reference_bit = NULL;
}

void mem_insert(int page_number, int* frame, mem_entry* victim_page, int pid){
    // printf("inserting to memory \n");
    /*find if there is an empty frame*/
    bool found_empty_frame = false;
    for (int i=0; i<nframes; i++){
        if(mmframe[i].page_number == -1){
            // printf("Found empty frame. Inserting here\n");
            time[i] = timecounter;
            found_empty_frame = true;
            mmframe[i].page_number = page_number;
            mmframe[i].pid = pid;
            *frame = i;
            victim_page->page_number = -1;
            break;
        }      
    } 
    if (found_empty_frame == false){
        // printf("Replacing...\n");
        /*run replacement algorithm*/
        int pos = replace_alg(); 
        time[pos] = timecounter;
        *frame = pos;
        *victim_page = mmframe[pos];
        mmframe[pos].page_number = page_number;
        mmframe[pos].pid = pid;
    }
    return;
}

int findLRU(){
    int minimum = time[0], frame_victim = 0;
 
	for(int i = 1; i < nframes; i++){
		if(time[i] < minimum){
			minimum = time[i];
			frame_victim = i;
		} else if (time[i] == minimum){
            printf("same times");
            exit(-1);
        }
	}
    return frame_victim;
}

int secondchance(){
    /**/
    int frame_victim;
    while(1){ /*loop until a victim is found*/
        if (reference_bit[pointer] == false) {
            frame_victim = pointer;
            reference_bit[pointer] = true;
            pointer = (pointer+1)%nframes;
            return frame_victim;
        }
        reference_bit[pointer] = false;
        pointer = (pointer+1)%nframes;
    }
} 

void mem_print(){
    printf("Memory now\n");
    for (int i=0; i<nframes-1; i++){
        printf("(%5d,%d) ", mmframe[i].page_number, mmframe[i].pid);
    }
    printf("(%5d,%d)\n", mmframe[nframes-1].page_number, mmframe[nframes-1].pid);
    
    if (replace_alg == findLRU){
        for (int i=0; i<nframes-1; i++){
            printf("%5d     ", time[i]);
        }
        printf("%5d     \n", time[nframes-1]);
    }
    if (replace_alg == secondchance){
        for (int i=0; i<nframes-1; i++){
            printf("%5d     ", reference_bit[i]);
        }
        printf("%5d\n", reference_bit[nframes-1]);
        printf("ptr=%d\n", pointer);
    }
}