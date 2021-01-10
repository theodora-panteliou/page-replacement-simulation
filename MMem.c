#include "MMem.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int pgfaultcnt;
static int nframes;
static mem_entry* mmframe = NULL;
int timecounter = 0;
int* time;
static int (*replace_alg)();
static int victimptr=0;

extern char alg[4];

int findLRU();
int secondchance();

void mem_initialize(int nframesk) {
    nframes = nframesk;
    if (mmframe == NULL) {
        mmframe = malloc(nframes*sizeof(mem_entry));
        time = malloc(nframes*sizeof(int)); /*for lru*/
        usedbit = malloc(nframes*sizeof(bool)); /*for 2nd chance*/
    }
    timecounter = 0;
    pgfaultcnt=0;
    for(int i=0; i<nframes; i++) {
        mmframe[i].page_number=-1;
        mmframe[i].pid=-1;
        time[i] = 0; /*for lru*/
        usedbit[i] = 0; /*for 2nd chance*/
    }
    printf("alg %s\n", alg);
    if (strcmp(alg, "LRU") == 0){
        printf("USING LRU\n");
        replace_alg = findLRU;
    }
    if (strcmp(alg, "2C") == 0){
        printf("USING 2C\n");
        replace_alg = secondchance;
    }
}

void mem_delete(){
    free(mmframe);
    mmframe = NULL;

    free(time);
    time = NULL;

    free(usedbit);
    usedbit = NULL;
}

void mem_insert(int page_number, int* frame, mem_entry* victim_page, int pid){
    printf("inserting to memory \n");
    /*find if there is an empty frame*/
    bool found_empty_frame = false;
    for (int i=0; i<nframes; i++){
        if(mmframe[i].page_number == -1){
            printf("Found empty frame. Inserting here\n");
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
        printf("Replacing...\n");
        /*run replacement algorithm*/
        int pos = replace_alg(); 
        time[pos] = timecounter;;
        *frame = pos;
        *victim_page = mmframe[pos];
        mmframe[pos].page_number = page_number;
        mmframe[pos].pid = pid;
    }
    return;
}

int findLRU(){
    int minimum = time[0], frame_victim = 0;
 
	for(int i = 1; i < nframes; ++i){
		if(time[i] < minimum){
			minimum = time[i];
			frame_victim = i;
		}
	}
    return frame_victim;
}

int secondchance(){
    /**/
    int frame_victim;
    if(usedbit[victimptr]==1){ 
        /*if reference bit is 1 continue until 
        a frame with reference bit 0 is found*/
        do{
            usedbit[victimptr]=0; 
            victimptr++;
            if(victimptr==nframes)
                victimptr=0;
        }while(usedbit[victimptr]!=0);
    }

    if(usedbit[victimptr]==0){ /*if reference bit is 0*/

        frame_victim = victimptr; /*found the victim*/
        usedbit[victimptr]=1; /*new page in frame->ref bit is 1*/
        victimptr++;
    }
    if(victimptr==nframes) 
        victimptr=0;
    return frame_victim;
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
            printf("%5d     ", usedbit[i]);
        }
        printf("%5d\n", usedbit[nframes-1]);
        printf("ptr=%d\n", victimptr);
    }
}