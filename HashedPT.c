#include "HashedPT.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

static int HPT_SIZE;
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
    return page_number%HPT_SIZE; /* simple hash function */
}

HashedPT HashedPT_init(int nframes){
    HPT_SIZE = nframes;
    HashedPT page_table = malloc(sizeof(HashedPT_entry*)*HPT_SIZE);
    for (int i=0; i < HPT_SIZE; i++){
        page_table[i] = NULL;
    }
    return page_table;
}

void HashedPT_insert(HashedPT page_table, int page_number, int frame, char rw){
    assert(page_table != NULL);

    assert(frame<HPT_SIZE && frame>=0); //invalid frame

    int hash_value = HashedPT_HashFunction(page_number);
    // printf("hash %d\n", hash_value);

    if (page_table[hash_value] != NULL){ 
        HashedPT_entry* curr = page_table[hash_value], *prev = NULL;
        
        do {
            prev = curr;
            curr = curr->next;
        } while (curr!= NULL);
        
        prev->next = create_entry(page_number, frame, rw);
    }
    else {
        page_table[hash_value] = create_entry(page_number, frame, rw);
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
    assert(curr!=NULL);
    if (curr != NULL && curr->page_number == page_number){
        if (curr->dirty == true) {
            *writes+=1;
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
    assert(curr != NULL);
    if (curr->dirty == true) {
        *writes+=1;
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

bool HashedPT_Hit(HashedPT page_table, int page_number) {
    HashedPT_entry* entry = HashedPT_getEntry(page_table, page_number);
    if (entry == NULL){
        return false;
    } else {
        entry->reference = true; /*Update reference for 2nd chance*/
        entry->time = timecounter; /*Upadte time for LRU*/
        return true;
    }
}

void HashedPT_set_reference(HashedPT page_table, int page_number, bool value) {
    HashedPT_entry* entry = HashedPT_getEntry(page_table, page_number);
    entry->reference = value;
}

bool HashedPT_get_reference(HashedPT page_table, int page_number) {
    HashedPT_entry* entry = HashedPT_getEntry(page_table, page_number);
    return entry->reference;
}

int HashedPT_get_time(HashedPT page_table, int page_number) {
    HashedPT_entry* entry = HashedPT_getEntry(page_table, page_number);
    return entry->time;
}

void HashedPT_set_dirty(HashedPT page_table, int page_number) {
    HashedPT_entry* entry = HashedPT_getEntry(page_table, page_number);
    entry->dirty = true;
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
                printf("\tpage_number:%d, frame_number:%d, dirty_bit:%d refernced:%d time:%d\n", curr->page_number, curr->frame_number, curr->reference, curr->dirty, curr->time);
                curr = curr->next;
            }
        }
    }
    return;
}