#include "HashedPT.h"
#include <stdio.h>
#include <stdlib.h>

int HPT_SIZE;
struct HashedPT_entry{
    int page_number;
    int frame_number;
    bool present; /*1 for valud, 0 for unvalid*/
    bool dirty;
    struct HashedPT_entry* next;
};

int HashedPT_HashFunction(long int page_number){
    return page_number%HPT_SIZE;
}

HashedPT HashedPT_init(int nframes){
    HPT_SIZE = nframes;
    HashedPT page_table = malloc(sizeof(HashedPT_entry*)*HPT_SIZE);
    for (int i=0; i < HPT_SIZE; i++){
        page_table[i] = NULL;
    }  /*valgrind //FIXME */
    return page_table;
}

extern int linesread;

void HashedPT_insert(HashedPT page_table, int frame, int page_number, char rw){
    if (page_table == NULL) return;

    int hash_value = HashedPT_HashFunction(page_number);
    printf("hash %d\n", hash_value);

    if (page_table[hash_value] != NULL){ 
        printf("insert\n");
        HashedPT_entry* curr = page_table[hash_value], *prev = NULL;
        // if (curr->page_number == page_number && curr->frame_number == -1) { //update
        //     if (rw == 'r'){
        //         curr->dirty = false;
        //     } else {
        //         curr->dirty = true;
        // }
        // }
        // if (curr->page_number == page_number && curr->frame_number == frame) { 
        //     printf("already exists\n");
        //     return;
        // }
        do {
            if (curr->page_number == page_number && curr->present == false) {
                /*update current frame*/
                curr->frame_number = frame;
                curr->present  = true;
                if (rw == 'W'){
                    curr->dirty = true;
                } else if (rw == 'R'){
                    curr->dirty = false;
                } else {
                    printf("\t\t\tERROR\n");
                }
                return;
            } else if (curr->page_number == page_number && curr->present == true){
                if (rw == 'W'){
                    curr->dirty = true;
                }
                return;
            } else {
                printf("\t\t\tALREADY HERE\n");
            }
            prev = curr;
            curr = curr->next;
        } while (curr!= NULL);
        
        HashedPT_entry* new_entry = malloc(sizeof(HashedPT_entry)); //FIXME:Uninitialised value was created by a heap allocation line 72 at line 117
        new_entry->page_number = page_number;
        new_entry->frame_number = frame;
        new_entry->present = true;
        if (rw == 'R' || rw == 'r'){
            new_entry->dirty = false;
        } else if (rw == 'W' || rw == 'w'){
            new_entry->dirty = true;
        } 
        else {
                    printf("rw %c", rw);
            printf("\t\t\tERROR\n"); //FIXME this wasn't supposed to happen 
            printf("lines read %d\n", linesread);
            exit(-1);
        }
        new_entry->next = NULL;

        prev->next = new_entry;
    }
    else {
        printf("insert first\n");

        HashedPT_entry* new_entry = malloc(sizeof(HashedPT_entry));
        new_entry->page_number = page_number;
        new_entry->frame_number = frame;
        new_entry->present = true;
        if (rw == 'R'){
            new_entry->dirty = false;
        } else if (rw == 'W'){
            new_entry->dirty = true;
        } 
        else {
            printf("\t\t\tERROR\n");
        }
        new_entry->next = NULL;

        page_table[hash_value] = new_entry;
    }
}

void HashedPT_setInvalid(HashedPT page_table, int page_number, int* writes){
    int hash_value = HashedPT_HashFunction(page_number);
    HashedPT_entry* curr = page_table[hash_value];
    do {
        if (curr->page_number == page_number) {
            printf("\t\tpage# %d %d\n", curr->page_number, page_number);
            curr->present = false;
            printf("\t\tbefore writes\n");
            if (curr->dirty == true) { /*valgrind //FIXME */
                *writes+=1;
                printf("\t\twrites %d %p\n", *writes, writes);
            }
        }
        curr = curr->next;
    } while (curr!= NULL);
}

void HashedPT_delete(HashedPT* page_table){
    for (int i = 0; i < HPT_SIZE; i++){
        if ((*page_table)[i] != NULL){
            HashedPT_entry* temp;
            while ((*page_table)[i]) {
                temp = (*page_table)[i];
                (*page_table)[i] = ((*page_table)[i])->next;
                free(temp);
                temp = NULL;
            }
        }
    }
    free(*page_table);
    *page_table = NULL;
}

extern unsigned int* time; 
extern unsigned int timecounter;

int Hit(HashedPT page_table, int page_number) {
    int hash_value = HashedPT_HashFunction(page_number);
    HashedPT_entry* curr = page_table[hash_value];
    if (curr != NULL) {
        do {
            if (curr->page_number == page_number) {
                printf("\tcurr->page_number %d\n", curr->page_number);
                if (curr->present == true){
                    time[curr->frame_number] = timecounter;  /*valgrind //FIXME */
                    return curr->frame_number;
                } else {
                    return -1;
                }
            }
            curr = curr->next;
        } while (curr != NULL);
    }
    return -1;
}