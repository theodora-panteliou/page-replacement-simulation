#include "HashedPT.h"
#include <stdio.h>
#include <stdlib.h>

int HPT_SIZE;
extern unsigned int timecounter;
struct HashedPT_entry{
    int page_number;
    int frame_number;
    bool dirty;
    bool reference;
    unsigned int time;
    struct HashedPT_entry* next;
};

HashedPT_entry* create_entry(int page_number, int frame, char rw);
HashedPT_entry* HashedPT_getEntry(HashedPT, int page_number);

int HashedPT_HashFunction(long int page_number){
    return page_number%HPT_SIZE;
}

HashedPT HashedPT_init(int nframes){
    HPT_SIZE = nframes;
    HashedPT page_table = malloc(sizeof(HashedPT_entry*)*HPT_SIZE);
    for (int i=0; i < HPT_SIZE; i++){
        page_table[i] = NULL;
    }
    return page_table;
}

void HashedPT_insert(HashedPT page_table, int frame, int page_number, char rw){
    if (page_table == NULL) return;

    if ((frame < 0 && frame!=-2) || frame >= HPT_SIZE){ //invalid frame
        printf("frame %d\n", frame);
        exit(-1);
    }

    int hash_value = HashedPT_HashFunction(page_number);
    // printf("hash %d\n", hash_value);

    if (page_table[hash_value] != NULL){ 
        // printf("insert\n");
        HashedPT_entry* curr = page_table[hash_value], *prev = NULL;
        
        do {
            if (curr->page_number == page_number){
                // printf("modify\n");
                if (rw == 'W'){
                    curr->dirty = true;
                }
                return;
            }
            prev = curr;
            curr = curr->next;
        } while (curr!= NULL);
        
        prev->next = create_entry(page_number, frame, rw);
    }
    else {
        page_table[hash_value] = create_entry(page_number, frame, rw);;
    }
}

HashedPT_entry* create_entry(int page_number, int frame, char rw){
    HashedPT_entry* new_entry = malloc(sizeof(HashedPT_entry));
    new_entry->page_number = page_number;
    new_entry->frame_number = frame;
    if (rw == 'R'){
        new_entry->dirty = false;
    } else if (rw == 'W'){
        new_entry->dirty = true;
    }
    new_entry->reference = true;
    new_entry->time = timecounter;
    new_entry->next = NULL;
    return new_entry;
}

void HashedPT_remove(HashedPT page_table, int page_number, int* writes){
    int hash_value = HashedPT_HashFunction(page_number);
    HashedPT_entry* curr = page_table[hash_value];
    HashedPT_entry* prev;
    if (curr == NULL) {
        printf("Invalid remove\n");
        exit(-1);
    }
    if (curr != NULL && curr->page_number == page_number){
        // printf("\t\tpage# %d %d\n", curr->page_number, page_number);
        if (curr->dirty == true) {
            *writes+=1;
            // printf("\t\twrites %d %p\n", *writes, writes);
        }
        page_table[hash_value] = curr->next;
        free(curr);
        curr = NULL;
        return;
    }
    while (curr != NULL && curr->page_number != page_number){
        prev = curr;
        curr = curr->next;
    }
    if (curr == NULL) {
        printf("Invalid remove\n");
        exit(-1);
    }
    // printf("\t\tpage# %d %d\n", curr->page_number, page_number);
    if (curr->dirty == true) {
        *writes+=1;
        // printf("\t\twrites %d %p\n", *writes, writes);
    }
    prev->next = curr->next;
    free(curr);
    curr = NULL;
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

HashedPT_entry* HashedPT_getEntry(HashedPT page_table, int page_number){
    int hash_value = HashedPT_HashFunction(page_number);
    HashedPT_entry* curr = page_table[hash_value];
    if (curr != NULL) {
        do {
            if (curr->page_number == page_number) {
                return curr;
            }
            curr = curr->next;
        } while (curr != NULL);
    }
    return  NULL;
}

bool Hit(HashedPT page_table, int page_number) {
    HashedPT_entry* entry = HashedPT_getEntry(page_table, page_number);
    // printf("\tcurr->page_number HIT %d\n", curr->page_number);
    if (entry == NULL){
        return false;
    } else {
        entry->reference = true; /*Update reference for 2nd chance*/
        entry->time = timecounter; /*Upadte time for LRU*/
        return true;
    }
}

void set_reference(HashedPT page_table, int page_number, bool value) {
    HashedPT_entry* entry = HashedPT_getEntry(page_table, page_number);
    entry->reference = value;
}

bool get_reference(HashedPT page_table, int page_number) {
    HashedPT_entry* entry = HashedPT_getEntry(page_table, page_number);
    return entry->reference;
}

int get_time(HashedPT page_table, int page_number) {
    HashedPT_entry* entry = HashedPT_getEntry(page_table, page_number);
    return entry->time;
}

void HashedPT_print(const HashedPT pt) {
    for (int i = 0; i < HPT_SIZE; i++){
        if (pt[i] == NULL){
            printf("Index %d empty\n", i);
        }
        else {
            HashedPT_entry* curr = pt[i];
            printf("Index %d\n", i);
            while (curr!=NULL) {
                printf("\tpage_number:%d, frame_number:%d, dirty_bit:%d time:%d\n", curr->page_number, curr->frame_number, curr->dirty, curr->time);
                curr = curr->next;
            }
        }
    }
    return;
}