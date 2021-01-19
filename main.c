#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "MMem.h"

extern int timecounter;

char alg[4] = "LRU";

int main(int argc, char* argv[]){
    
    int nframes = 100, q = 80, max=10000; /*q is number of references, max is max number of references to be read*/
    if (argc >= 4){
        strcpy(alg, argv[1]);
        nframes = atoi(argv[2]);
        q = atoi(argv[3]);
        max = -1;
    } 
    if (argc == 5) {
        max = atoi(argv[4]);
    }
    /*check if args are valid*/
    if (argc>5 || argc<4){
        printf("Invalid arguments\n");
        return 0;
    }
    if ((strcmp(alg, "2C")!=0) && (strcmp(alg, "LRU")!=0)) {
        printf("Invalid Algorithm. Try LRU or 2C\n");
        return 0;
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

    mem_initialize(nframes, alg); /*frames in main memory*/
    
    /*variables for reading input*/
    char *line; 
    size_t linesize = 12; /*line in trace has 8 hexadecimal digits, 1 space, 1 char R or W and \n*/
    line = malloc(linesize*sizeof(char));
    char ref[9];
    char rw; /*read or write*/

    /*counters*/
    int num_references = 0;

    bool stop = false;
    unsigned int iaddress;
    int page_number;

    while (1){
        
        for (int i = 0; i < q; i++){
            timecounter++;
            if (getline(&line, &linesize, fp_gcc) == -1) {
                stop = true;
                break;
            }
            num_references++;
            if (num_references >= max && max>=0) break;

            strncpy(ref, line, 8);
            ref[8] = '\0';
            rw = line[9];
            /**/
            iaddress = strtol(ref, NULL, 16);
            page_number = iaddress >> OFFSET_SIZE; /*get rid off offset bytes to get page number*/
            // printf("page number is %d\n", page_number);
            mem_insert(page_number, PID_GCC, rw);
            // mem_print();
        }
        
        for (int i = 0; i < q; i++){
            timecounter++;
            if (getline(&line, &linesize, fp_bzip) == -1){
                break;
                stop = true;
            }
            num_references++;
            if (num_references >= max && max>=0) break;

            strncpy(ref, line, 8);
            ref[8] = '\0'; /*reference without W or R*/
            rw = line[9]; /*write or read char*/
            /**/
            iaddress = strtol(ref, NULL, 16);
            page_number = iaddress >> OFFSET_SIZE; /*get rid off offset bytes to get page number*/
            // printf("page number %d\n", page_number);
            mem_insert(page_number, PID_BZIP, rw);
            // mem_print();
        }
        
        if ((num_references >= max && max >= 0) || stop == true) break;
        
    }

    printf("Algorithm: %s\n", alg);
    print_stats();
    printf("%d references were examined\n", num_references);
    printf("frames: %d q: %d\n", nframes, q);

    fclose(fp_bzip);
    fp_bzip = NULL;

    fclose(fp_gcc);
    fp_gcc = NULL;

    mem_delete();

    free(line);
    line = NULL;

    return 0;
}