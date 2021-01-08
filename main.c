#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "HashedPT.h"
#include "MMem.h"

int main(int argc, char* argv[]){
    
    char alg[4] = "LRU";
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
    printf("%s %d %d\n", alg, nframes, q);
    MMem_initialize(nframes); /*frames in main memory*/

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
    
    HashedPT HPTbzip = NULL, HPTgcc = NULL;
    HPTbzip = HashedPT_init();
    HPTgcc = HashedPT_init();
    printf("init done %p\n", HPTbzip);

    char *line; /*line in trace has 8 hexadecimal digits, 1 space, 1 char R or W and \n*/
    size_t linesize = 12;
    line = malloc(linesize*sizeof(char));
    char ref[9];
    char rw;
    bool stop = false;

    unsigned int pgfault = 0;
    int num_references = 0;
    while (1){
        for (int i = 0; i < q; i++){

            if (getline(&line, &linesize, fp_bzip) == -1){
                break;
                stop = true;
            } 
            num_references++;
            if (num_references >= max) break;
            printf("\nbzip line %s", line);
            strncpy(ref, line, 9);
            ref[9] = '\0';
            rw = line[10];
            /**/
            unsigned int iaddress = strtol(ref, NULL, 16);
            long int page_number = iaddress >> OFFSET_SIZE; /*get rid of offset bytes to get page number*/
            printf("page number %d\n", page_number);
            if (MMem_isHit(page_number) == false){
                printf("page number not in memory\n");
                pgfault++;
                MMem_insert(ref, HPTbzip);
    		} else printf("page number already in memory\n");

        }
        for (int i = 0; i < q; i++){
            if (getline(&line, &linesize, fp_gcc) == -1) {
                stop = true;
                break;
            }
            num_references++;
            if (num_references >= max) break;
            printf("\ngcc line %s", line);
            strncpy(ref, line, 9);
            ref[9] = '\0';
            rw = line[10];
            /**/
            unsigned int iaddress = strtol(ref, NULL, 16);
            long int page_number = iaddress >> OFFSET_SIZE; /*get rid of offset bytes to get page number*/
            if (MMem_isHit(page_number) == false){
                pgfault++;
                MMem_insert(ref, HPTbzip);
    		} else printf("page number already in memory\n");
        }
        if (num_references >= max || stop == true) break;
        
    }

    printf("Page fault count is %d\n", pgfault);
    // printf("Read from disc count is %d\n", writes);
    // printf("Write to disc count is %d\n", reads);
    printf("%d refences were examined\n", num_references);
    printf("frames: %d q: %d\n", nframes, q);

    MMem_delete();

    free(line);
    line = NULL;

    HashedPT_delete(&HPTbzip);
    HashedPT_delete(&HPTgcc);

    fclose(fp_bzip);
    fp_bzip = NULL;

    fclose(fp_gcc);
    fp_bzip = NULL;
    
    return 0;
}