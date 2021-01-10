#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "HashedPT.h"
#include "MMem.h"

extern int timecounter;
    int linesread = 0;

char alg[4] = "LRU";

int main(int argc, char* argv[]){
    
    int nframes = 50, q = 6, max=10; /*q is number of references*/
    if (argc >= 4){
        strcpy(alg, argv[1]);
        nframes = atoi(argv[2]);
        q = atoi(argv[3]);
    } 
    if (argc == 5) {
        max = atoi(argv[4]);
        printf("%d\n", max);
    }

    /*open traces*/
    FILE *fp_bzip, *fp_gcc;
    fp_bzip = fopen("bzip.trace", "r");
    if (fp_bzip == NULL){
        printf("Unable to read file\n");
        return 1;
    }
    fp_gcc = fopen("gcc.trace", "r");
    if (fp_gcc == NULL){
        printf("Unable to read file\n");
        return 1;
    }

    printf("%s %d %d\n", alg, nframes, q);

    mem_initialize(nframes); /*frames in main memory*/
    
    HashedPT HPTbzip = NULL, HPTgcc = NULL;
    HPTbzip = HashedPT_init(nframes);
    HPTgcc = HashedPT_init(nframes);

    /*variables for reading input*/
    char *line; 
    size_t linesize = 12; /*line in trace has 8 hexadecimal digits, 1 space, 1 char R or W and \n*/
    line = malloc(linesize*sizeof(char));
    char ref[9];
    char rw; /*read or write*/

    /*counters*/
    unsigned int pgfault = 0;
    int num_references = 0;
    int reads = 0;
    int writes = 0;
    printf("writes init %p\n", &writes);

    bool stop = false;
    unsigned int iaddress;
    int frame_number = -4, page_number;
    mem_entry victim_page;
    int pidbzip = 1;
    int pidgcc = 2;
    int hit;

    while (1){
        for (int i = 0; i < q; i++){
            timecounter++;
            if (getline(&line, &linesize, fp_bzip) == -1){
                break;
                stop = true;
            } 
            linesread ++;
            num_references++;
            if (num_references >= max) break;

            printf("\nbzip line %s", line);
            strncpy(ref, line, 8);
            ref[8] = '\0';
            rw = line[9];
            /**/
            iaddress = strtol(ref, NULL, 16);
            page_number = iaddress >> OFFSET_SIZE; /*get rid of offset bytes to get page number*/
            printf("page number %d\n", page_number);
            hit = Hit(HPTbzip, page_number);
            if (hit == -1){
                printf("page number not in memory\n");
                pgfault++;
                reads++;
                mem_insert(page_number, &frame_number, &victim_page, pidbzip);
                /*insert page number into page table in frame */
                HashedPT_insert(HPTbzip, frame_number, page_number, rw);
                if (victim_page.page_number != -1){
                    printf("victim page %d %d\n", victim_page.page_number, victim_page.pid);
                    if (victim_page.pid == pidbzip) {
                        HashedPT_setInvalid(HPTbzip, victim_page.page_number, &writes);
                    } else if (victim_page.pid == pidgcc) {
                        HashedPT_setInvalid(HPTgcc, victim_page.page_number, &writes);
                    }
                } 
                
    		} else {
                printf("page number already in memory\n");
                usedbit[hit] = 1;
                if (rw == 'W')
                    HashedPT_insert(HPTbzip, -2, page_number, 'W');
            }
            mem_print();

        }
        for (int i = 0; i < q; i++){
            timecounter++;
            if (getline(&line, &linesize, fp_gcc) == -1) {
                stop = true;
                break;
            }
            num_references++;
            if (num_references >= max) break;

            printf("\ngcc line %s", line);
            strncpy(ref, line, 8);
            ref[8] = '\0';
            rw = line[9];
            /**/
            iaddress = strtol(ref, NULL, 16);
            page_number = iaddress >> OFFSET_SIZE; /*get rid of offset bytes to get page number*/
            printf("page number is %d\n", page_number);
            hit = Hit(HPTgcc, page_number);
            if (hit == -1){
                pgfault++;
                reads++;
                mem_insert(page_number, &frame_number, &victim_page, pidgcc);
                /*insert page number into page table in frame */
                HashedPT_insert(HPTgcc, frame_number, page_number, rw);
                if (victim_page.page_number != -1){
                    printf("victim page %d %d\n", victim_page.page_number, victim_page.pid);
                    if (victim_page.pid == pidbzip) {
                        HashedPT_setInvalid(HPTbzip, victim_page.page_number, &writes);
                    } 
                    else if (victim_page.pid == pidgcc) {
                        HashedPT_setInvalid(HPTgcc, victim_page.page_number, &writes);
                    }
                }
    		} else {
                printf("page number already in memory\n");
                usedbit[hit] = 1;
                if (rw == 'W')
                    HashedPT_insert(HPTgcc, -2, page_number, 'W');
            }
            mem_print();
        }
        if (num_references >= max || stop == true) break;
        
    }

    printf("Page fault count is %d\n", pgfault);
    printf("Read from disc count is %d\n", reads);
    printf("Write to disc count is %d\n", writes);
    printf("%d refences were examined\n", num_references);
    printf("frames: %d q: %d\n", nframes, q);

    fclose(fp_bzip);
    fp_bzip = NULL;

    fclose(fp_gcc);
    fp_gcc = NULL;

    mem_delete();

    free(line);
    line = NULL;

    HashedPT_delete(&HPTbzip);
    HashedPT_delete(&HPTgcc);

    return 0;
}