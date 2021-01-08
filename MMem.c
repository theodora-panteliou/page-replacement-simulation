#include "MMem.h"
#include <stdio.h>
#include <stdlib.h>

static int pgfaultcnt;
static int nframes;
static int* mmframe = NULL;
static int timecounter = 0;
static int* time;

void MMem_initialize(int nframesk) {
    nframes = nframesk;
    if (mmframe == NULL) {
        mmframe = malloc(nframes*sizeof(int));
        time = malloc(nframes*sizeof(int));
    }
    timecounter = 0;
    pgfaultcnt=0;
    for(int i=0; i<nframes; i++) {
        mmframe[i]=-1;
        time[i] = 0;
    }
}

bool MMem_isHit(int data) {
    
    for(int j=0; j<nframes; j++){
        if(mmframe[j]==data){
            timecounter++;
            time[j] = timecounter;
            return true;
        }
    }
    return false;
}

int MMem_getHitIndex(int data)
{
    int hitind;
    for(int k=0; k<nframes; k++)
    {
        if(mmframe[k]==data)
        {
            hitind=k;
            break;
        }
    }
    return hitind;
}

void MMem_dispPages()
{
    for (int k=0; k<nframes; k++){
        if(mmframe[k]!=-1)
            printf(" %d",mmframe[k]);
    }

}

void MMem_delete(){
    free(mmframe);
    mmframe = NULL;

    free(time);
    time = NULL;
}

void MMem_insert(char* ref, HashedPT pt){
    printf("inserting to memory \n");
    /*find if there is an empty frame*/
    unsigned int iaddress = strtol(ref, NULL, 16);
    long int page_number = iaddress >> OFFSET_SIZE; /*get rid of offset bytes to get page number*/
    int pos; 
    int victim_pg;
    bool found_empty_frame = false;
    for (int i=0; i<nframes; i++){
        if(mmframe[i] == -1){
            printf("Found empty frame. Inserting here\n");
            timecounter++;
            time[i] = timecounter;
            found_empty_frame = true;
            /*insert page number into page table in frame i*/
            HashedPT_insert(pt, i, ref, 'r');
            mmframe[i] = page_number;
            break;
        }      
    } 
    if (found_empty_frame == false){
        printf("Replacing...\n");
        /*run replacement algorithm*/
        timecounter++;
        pos = findLRU(); 
        HashedPT_insert(pt, pos, ref, 'r');
        time[pos] = timecounter;
        victim_pg = mmframe[pos];
        printf("old pg %d\n", victim_pg);
        HashedPT_setInvalid(pt, victim_pg);
    }
    return;
}

// void lru()
// {
//     initialize();

//     int least[nframes];
//     for(int i=0; i<n; i++){ //for sequence of references
//         printf("\nFor %d :",in[i]);

//         if(isHit(in[i])==0){
//             for(int j=0; j<nframes; j++){

//                 int pg=mmframe[j];
//                 int found=0;
//                 for(int k=i-1; k>=0; k--){
//                     if(pg==in[k]){
//                         least[j]=k;
//                         found=1;
//                         break;
//                     }
//                     else
//                         found=0;
//                 }
//                 if(!found)
//                     least[j]=-9999;
//             }
//             int min=9999;
//             int repindex;
//             for(int j=0; j<nframes; j++){
//                 if(least[j]<min){
//                     min=least[j];
//                     repindex=j;
//                 }
//             }
//             mmframe[repindex]=in[i];
//             pgfaultcnt++;

//             dispPages();
//         }
//         else
//             printf("No page fault!");
//     }
//     dispPgFaultCnt();
// }

int findLRU(){
    int i, minimum = time[0], pos = 0;
 
	for(i = 1; i < nframes; ++i){
		if(time[i] < minimum){
			minimum = time[i];
			pos = i;
		}
	}
	
	return pos;
}

// void lru(char* ref, int* recent){ /*Least recently used*/
//     initialize();

//     int least[nframes];
//     for(int i=0; i<n; i++){ //for sequence of references
//         printf("\nFor %s :", ref);
//         unsigned int iaddress = strtol(ref, NULL, 16);
//         long int page_number = iaddress >> OFFSET_SIZE; /*get rid of offset bytes to get page number*/

//         if(isHit(page_number)==false){ //page not in main memory
//             for(int j=0; j<nframes; j++){
//                 int pg=mmframe[j];
//                 bool found=false;
//                 for(int k=i-1; k>=0; k--){
//                     if(pg==page_number){
//                         recent[j]=k;
//                         found=true;
//                         break;
//                     }
//                     else
//                         found=false;
//                 }
//                 if(!found)
//                     recent[j]=-9999;
//             }
//             int min=9999;
//             int repindex;
//             for(int j=0; j<nframes; j++){
//                 if(recent[j]<min){
//                     min=recent[j];
//                     repindex=j;
//                 }
//             }
//             mmframe[repindex]=page_number;
//             pgfaultcnt++;

//             dispPages();
//         }
//         else
//             printf("No page fault!");
//     }
//     dispPgFaultCnt();
// }

/*void secondchance()
{
    int usedbit[50];
    int victimptr=0;
    initialize();
    for(int i=0; i<nframes; i++)
        usedbit[i]=0;
    for(int i=0; i<n; i++)
    {
        printf("\nFor %d:",in[i]);
        if(isHit(in[i]))
        {
            printf("No page fault!");
            int hitindex=getHitIndex(in[i]);
            if(usedbit[hitindex]==0)
                usedbit[hitindex]=1;
        }
        else
        {
            pgfaultcnt++;
            if(usedbit[victimptr]==1)
            {
                do
                {
                    usedbit[victimptr]=0;
                    victimptr++;
                    if(victimptr==nframes)
                        victimptr=0;
                }
                while(usedbit[victimptr]!=0);
            }
            if(usedbit[victimptr]==0)
            {
                mmframe[victimptr]=in[i];
                usedbit[victimptr]=1;
                victimptr++;
            }
            dispPages();

        }
        if(victimptr==nframes)
            victimptr=0;
    }
    dispPgFaultCnt();
} */