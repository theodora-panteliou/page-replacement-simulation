#ifndef HASHEDPT_H
#define HASHEDPT_H

#include <stdbool.h>

typedef struct HashedPT_entry HashedPT_entry;
typedef HashedPT_entry** HashedPT; /*array of pointers to pt entries*/

HashedPT HashedPT_init(int nframes);

void HashedPT_insert(HashedPT page_table, int frame, int page_number, char rw); 

int Hit(HashedPT page_table, int page_number); /*returns frame number if hit*/

void HashedPT_delete(HashedPT* page_table);

void HashedPT_remove(HashedPT page_table, int page_number, int* writes);

void HashedPT_print(const HashedPT pt);

bool get_reference(HashedPT page_table, int page_number);

void set_reference(HashedPT page_table, int page_number, bool value);

#endif