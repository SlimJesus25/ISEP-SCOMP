#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/syscall.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <sys/wait.h>
#include <ctype.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <math.h>
#include <semaphore.h>
#include "../../semaphore_utils.h"

#define MUTEX_NUMBER 2
#define FIRST 0
#define SECOND 1
#define CHILDREN 8
#define INTEGERS_TO_READ 200
#define NUMBERS "Numbers.txt"
#define OUTPUT "Output.txt"
#define MUTEX_NAME "/mutex"
#define SYNC_NAME "/sync"

/**
 * 
 *  
*/

int main(){

    sem_t* mutexes[MUTEX_NUMBER];
    sem_t* syncs[CHILDREN];
    create_semaphores(mutexes, MUTEX_NUMBER, MUTEX_NAME, MUTEX_INITAL_VALUE);
    create_semaphores(syncs, CHILDREN, SYNC_NAME, SYNCHRONIZATION_INITAL_VALUE);

    sem_post(syncs[FIRST]);

    remove(OUTPUT);

    pid_t p;
    for(int i=0;i<CHILDREN;i++){
        p = fork();
        if(p < 0){
            perror("fork");
            exit(EXIT_FAILURE);
        }
        if(p == 0){

            sem_wait(syncs[i]);

            sem_wait(mutexes[FIRST]);

            FILE* rfptr;

            rfptr = fopen(NUMBERS, "r");
            if(rfptr == NULL){
                perror("fopen");
                exit(EXIT_FAILURE);
            }

            int numbers[INTEGERS_TO_READ], j=0;
            while(j < INTEGERS_TO_READ){
                fscanf(rfptr, "%d", &numbers[j++]);
            }

            fclose(rfptr);
            sem_post(mutexes[FIRST]);
            
            if(i != CHILDREN-1)
                sem_post(syncs[i+1]);

            sem_wait(mutexes[SECOND]);
            FILE* wfptr;

            wfptr = fopen(OUTPUT, "a");
            if(wfptr == NULL){
                perror("fopen");
                exit(EXIT_FAILURE);
            }

            for(j = 0; j < INTEGERS_TO_READ; j++){
                fprintf(wfptr, "[%d] %d\n", getpid(), numbers[j]);
            }

            fclose(wfptr);

            sem_post(mutexes[SECOND]);
            exit(EXIT_SUCCESS);
        }
    }

    for(int i=0;i<CHILDREN;i++){
        wait(NULL);
    }

    FILE* fptr;

    fptr = fopen(OUTPUT, "r");
    if(fptr == NULL){
        perror("fopen");
        exit(EXIT_FAILURE);
    }


    for(int i=0;i<CHILDREN*INTEGERS_TO_READ;i++){
        char line[INTEGERS_TO_READ];
        fscanf(fptr, "%s", line);
        printf("%s", line);
        fscanf(fptr, "%s", line);
        printf("%s\n", line);
    }

    unlink_semaphores(MUTEX_NAME, MUTEX_NUMBER);
    unlink_semaphores(SYNC_NAME, CHILDREN);

    exit(EXIT_SUCCESS);
}