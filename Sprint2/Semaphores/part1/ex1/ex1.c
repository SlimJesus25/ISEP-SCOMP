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

#define CHILDREN 8
#define INTEGERS_TO_READ 200
#define NUMBERS "Numbers.txt"
#define OUTPUT "Output.txt"
#define MUTEX_NAME "/mutex"
#define BARRIER_NAME "/barrier"
#define LIMITED_RESOURCES_NAME "/limited_resources"
#define SHARED_MEMORY_NAME "/shared_memory"

/**
 * 
 *  
*/

int main(){

    sem_t* mutex = sem_open(MUTEX_NAME, O_CREAT, S_IRUSR|S_IWUSR, MUTEX_INITAL_VALUE);

    remove(OUTPUT);

    pid_t p;
    for(int i=0;i<CHILDREN;i++){
        p = fork();
        if(p < 0){
            perror("fork");
            exit(EXIT_FAILURE);
        }
        if(p == 0){
            sem_wait(mutex);

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

            sem_post(mutex);
            exit(EXIT_SUCCESS);
        }
    }

    for(int i=0;i<CHILDREN;i++){
        wait(NULL);
    }

    if(sem_unlink(MUTEX_NAME) < 0){
        perror("sem_unlink");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}