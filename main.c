#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "HashedPT.h"

void initialize();
int isHit(int data);
int getHitIndex(int data);
void dispPages();

int main(int argc, char* argv[]){
    
    char alg[4] = "LRU";
    int nframes = 50, q = 6, max=0; /*q is number of references*/
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
    int* mmframe = malloc(sizeof(int)*nframes); /*frames in main memory*/

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
    
    HashedPT HPT1 = NULL, HPT2 = NULL;
    HPT1 = HashedPT_init();
    HPT2 = HashedPT_init();
    printf("init done %p\n", HPT1);

    char *line; /*line in trace has 8 hexadecimal digits, 1 space, 1 char R or W and \n*/
    size_t linesize = 12;
    line = malloc(linesize*sizeof(char));
    char ref[9];
    char rw;

    while (1){
        for (int i = 0; i < q; i++){
            
            getline(&line, &linesize, fp_bzip);
            printf("line %s\n", line);
            strncpy(ref, line, 9);
            ref[9] = '\0';
            rw = line[10];
            /**/
            HashedPT_insert(HPT1, ref);
            HashedPT_insert(HPT1, ref);
            HashedPT_insert(HPT1, ref);
            break;
        }
        break;

    }
    free(mmframe);
    mmframe = NULL;

    free(line);
    line = NULL;

    HashedPT_delete(&HPT1);
    HashedPT_delete(&HPT2);
    HashedPT_insert(HPT1, ref);

    fclose(fp_bzip);
    fp_bzip = NULL;

    fclose(fp_gcc);
    fp_bzip = NULL;
    
    return 0;
}




// void initialize()
// {
//     pgfaultcnt=0;
//     for(i=0; i<nf; i++)
//         p[i]=9999;
// }

// int isHit(int data)
// {
//     hit=0;
//     for(j=0; j<nf; j++)
//     {
//         if(p[j]==data)
//         {
//             hit=1;
//             break;
//         }

//     }

//     return hit;
// }

// int getHitIndex(int data)
// {
//     int hitind;
//     for(k=0; k<nf; k++)
//     {
//         if(p[k]==data)
//         {
//             hitind=k;
//             break;
//         }
//     }
//     return hitind;
// }

// void dispPages()
// {
//     for (k=0; k<nf; k++)
//     {
//         if(p[k]!=9999)
//             printf(" %d",p[k]);
//     }

// }