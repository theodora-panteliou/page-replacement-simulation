#include "HashedPT.h"
#include <stdio.h>
#include <stdlib.h>

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

HashedPT HashedPT_init(){
    HashedPT page_table = malloc(sizeof(HashedPT_entry*)*HPT_SIZE);
    printf("pt init %p\n", page_table);
    for (int i=0; i < HPT_SIZE; i++){
        page_table[i] = NULL;
    }
    return page_table;
}

void HashedPT_insert(HashedPT page_table, int frame, void* address, char rw){
    if (page_table == NULL) return;

    unsigned int iaddress = strtol(address, NULL, 16);
    long int page_number = iaddress >> OFFSET_SIZE; /*get rid of offset bytes to get page number*/

    int hash_value = HashedPT_HashFunction(page_number);
    printf("hash %d\n", hash_value);

    if (page_table[hash_value] != NULL){ 
        printf("insert\n");
        HashedPT_entry* curr = page_table[hash_value];
        if (curr->page_number == page_number && curr->frame_number == frame) {
            printf("already exists\n");
            return;
        }
        while (curr->next != NULL) {
            if (curr->page_number == page_number && curr->frame_number == frame) {
                printf("already exists\n");
                return;
            } else if (curr->page_number == page_number) {
                /*update current frame*/
                curr->frame_number = frame;
                curr->present  = true;
            }
            curr = curr->next;
        }
        
        HashedPT_entry* new_entry = malloc(sizeof(HashedPT_entry));
        new_entry->page_number = page_number;
        new_entry->frame_number = frame;
        new_entry->present = true;
        if (rw == 'r'){
            new_entry->dirty = false;
        } else {
            new_entry->dirty = true;
        }
        new_entry->next = NULL;

        curr->next = new_entry;
    }
    else {
        printf("insert first\n");

        HashedPT_entry* new_entry = malloc(sizeof(HashedPT_entry));
        new_entry->page_number = page_number;
        new_entry->frame_number = frame;
        new_entry->present = true;
        if (rw == 'r'){
            new_entry->dirty = false;
        } else {
            new_entry->dirty = true;
        }
        new_entry->next = NULL;

        page_table[hash_value] = new_entry;
    }
}

void HashedPT_setInvalid(HashedPT page_table, int page_number){
    int hash_value = HashedPT_HashFunction(page_number);
    HashedPT_entry* curr = page_table[hash_value];
    while (curr->next != NULL) {
        if (curr->page_number == page_number) {
            curr->present = false;
        }
        curr = curr->next;
    }
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

