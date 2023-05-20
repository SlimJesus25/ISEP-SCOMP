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
#include <semaphore.h>
#include <math.h>
#include "../../semaphore_utils.h"

#define SYNC "/sync5"
#define SEMAPHORE_QUANTITY 3
#define CHILDREN 3
#define FIRST 0

void print_and_post(char info[6][25], int iteration, sem_t* syncs[CHILDREN]){
    sem_wait(syncs[iteration]);
    printf("%s", info[iteration]);
    fflush(stdout);
    sem_post(syncs[(iteration+1)%CHILDREN]);
    
    sem_wait(syncs[iteration]);
    printf("%s", info[iteration+3]);
    fflush(stdout);
    sem_post(syncs[(iteration+1)%CHILDREN]);
}


/*
        ## Explicação ##
    A estratégia utilizada para resolver este exercício foi utilizar 3 semáforos (mínimos possíveis) de 
    sincronização entre processos (valor inicial igual a zero).
*/

int main(int argc, char *argv[]){

    char info[6][25] = {"Sistemas ", "de", " Computadores - ",
     "a ", "melhor ", "disciplina!"};

    sem_t* syncs[CHILDREN];
    create_semaphores(syncs, CHILDREN, SYNC, SYNCHRONIZATION_INITAL_VALUE);
    
    sem_post(syncs[FIRST]);
    //print_and_post(info, 0, syncs);


    for(int i=0;i<CHILDREN;i++){
        pid_t p = fork();
        if(p < 0){
            perror("fork");
            exit(EXIT_FAILURE);
        }

        if(p == 0){
            print_and_post(info, i, syncs);
            exit(EXIT_SUCCESS);
        }
    }

    for(int i=0;i<CHILDREN;i++){
        wait(NULL);
    }

    printf("\n");
    
    unlink_semaphores(SYNC, CHILDREN);

    exit(EXIT_SUCCESS);
}