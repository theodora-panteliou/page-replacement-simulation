#include "MMem.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "HashedPT.h"

typedef struct mem_entry{
    int page_number;
    int pid;
} mem_entry;

unsigned int timecounter = 0;

static int nframes;
static mem_entry* mmframe = NULL;
static int (*replace_alg)();
static int pointer=0;

HashedPT HPTbzip = NULL, HPTgcc = NULL;

static int pgfault, reads, writes;

/*declarations of functions that are used inside this file*/
int find_empty_frame();
int findLRU();
int secondchance();

HashedPT get_HPT(int pid);


void mem_initialize(int nframesk,char* alg) {
    nframes = nframesk;

    mmframe = malloc(nframes*sizeof(mem_entry));
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
    }
    HPTbzip = HashedPT_init(nframes);
    HPTgcc = HashedPT_init(nframes);
    reads = 0;
    writes = 0;
    pgfault = 0;
}

void mem_delete(){
    free(mmframe);
    mmframe = NULL;

    HashedPT_delete(&HPTbzip);
    HashedPT_delete(&HPTgcc);
}

void mem_insert(int page_number, int pid, char rw){
    // printf("inserting to memory \n");
    /*find if there is an empty frame*/
    
    if (Hit(get_HPT(pid), page_number) == false){
        pgfault++;
        reads++;

        /*insert*/
        int victim_frame;
        victim_frame = find_empty_frame();
        if (victim_frame == -1){
            // printf("Replacing...\n");
            /*run replacement algorithm*/
            int pos = replace_alg(); 
            victim_frame = pos;
            mem_entry victim_page;
            victim_page = mmframe[pos];
            mmframe[pos].page_number = page_number;
            mmframe[pos].pid = pid;

            /*remove victim page from page table*/
            // printf("victim page %d %d\n", victim_page.page_number, victim_page.pid);
            HashedPT_remove(get_HPT(victim_page.pid), victim_page.page_number, &writes);
        }
        
        mmframe[victim_frame].page_number = page_number;
        mmframe[victim_frame].pid = pid;
        /*insert page number into page table in frame */
        HashedPT_insert(get_HPT(pid), victim_frame, page_number, rw);

            
    } else {
        // printf("page number already in memory\n");
        if (rw == 'W'){
            HashedPT_insert(get_HPT(pid), -2, page_number, 'W');
        }
    }
}

int findLRU(){
    int minimum = timecounter, frame_victim = -1;
    int ptime;
	for(int i = 0; i < nframes; i++){
        ptime = get_time(get_HPT(mmframe[i].pid), mmframe[i].page_number);
		if (ptime < minimum){
			minimum = ptime;
			frame_victim = i;
		} else if (ptime == minimum){
            printf("same times %d %d\n", ptime, minimum);
            exit(-1);
        }
	}
    return frame_victim;
}

int secondchance(){
    /**/
    int frame_victim;
    while(1){ /*loop until a victim is found*/
        if (get_reference(get_HPT(mmframe[pointer].pid), mmframe[pointer].page_number) == false) {
            frame_victim = pointer;
            pointer = (pointer+1)%nframes;
            return frame_victim;
        }
        set_reference(get_HPT(mmframe[pointer].pid), mmframe[pointer].page_number, false);
        pointer = (pointer+1)%nframes;
    }
} 

int find_empty_frame(){
    for (int i=0; i<nframes; i++){
        if(mmframe[i].page_number == -1){
            // printf("Found empty frame. Inserting here\n");
            return i;
        }      
    }
    return -1;
}

HashedPT get_HPT(int pid){
    if (pid == PID_BZIP) {
        return HPTbzip;
    } 
    else if (pid == PID_GCC) {
        return HPTgcc;
    } else {
        printf("Invalid pid\n");
        exit(-1);
    }
}

void const print_stats(){
    printf("Page fault count is %d\n", pgfault);
    printf("Read from disk count is %d\n", reads);
    printf("Write to disk count is %d\n", writes);
}

void mem_print(){
    printf("Memory now\n");
    for (int i=0; i<nframes-1; i++){
        printf("(%5d,%d) ", mmframe[i].page_number, mmframe[i].pid);
    }
    printf("(%5d,%d)\n", mmframe[nframes-1].page_number, mmframe[nframes-1].pid);
    
    // if (replace_alg == findLRU){
    //     for (int i=0; i<nframes-1; i++){
    //         printf("%5d     ", time[i]);
    //     }
    //     printf("%5d     \n", time[nframes-1]);
    // }
    HashedPT_print(HPTbzip);
    HashedPT_print(HPTgcc);
    // if (replace_alg == secondchance){
    //     for (int i=0; i<nframes-1; i++){
    //         printf("%5d     ", reference_bit[i]);
    //     }
    //     printf("%5d\n", reference_bit[nframes-1]);
    //     printf("ptr=%d\n", pointer);
    // }
}