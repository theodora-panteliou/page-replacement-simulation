#ifndef HASHEDPT_H
#define HASHEDPT_H

#include <stdbool.h>

typedef struct HashedPT_entry HashedPT_entry;
typedef HashedPT_entry** HashedPT; /*array of pointers to pt entries*/

HashedPT HashedPT_init(int nframes);
void HashedPT_delete(HashedPT* page_table);

void HashedPT_insert(HashedPT page_table, int page_number, int frame, char rw); 
void HashedPT_remove(HashedPT page_table, int page_number, int* writes);

bool HashedPT_Hit(HashedPT page_table, int page_number); /*returns frame number if hit*/

void HashedPT_print(const HashedPT pt);

bool HashedPT_get_reference(HashedPT page_table, int page_number);
void HashedPT_set_reference(HashedPT page_table, int page_number, bool value);
int HashedPT_get_time(HashedPT page_table, int page_number);
void HashedPT_set_dirty(HashedPT page_table, int page_number);

#endif