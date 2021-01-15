#ifndef HASHEDPT_H
#define HASHEDPT_H

#include <stdbool.h>

/*sizes in bytes*/

#define PAGE_SIZE 4096
#define OFFSET_SIZE 12 /*log(PAGE_SIZE)*/
#define LOGICAL_SIZE 32
#define PAGE_NUM_SIZE LOGICAL_SIZE-OFFSET_SIZE /*total logical address size - offset size*/

// #define HPT_SIZE 200 /*num of PT's buckets*/

typedef struct HashedPT_entry HashedPT_entry;
typedef HashedPT_entry** HashedPT; /*array of pointers to pt entries*/

HashedPT HashedPT_init(int nframes);

void HashedPT_insert(HashedPT page_table, int frame, int page_number, char rw); 

int Hit(HashedPT page_table, int page_number);

void HashedPT_delete(HashedPT* page_table);

void HashedPT_remove(HashedPT page_table, int page_number, int* writes);

void HashedPT_print(const HashedPT pt);

#endif