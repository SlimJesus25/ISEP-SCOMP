#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <signal.h>
#include <sys/time.h>
#include <semaphore.h>

#define NLOOPS 1000     //para evitar o loop infinito

int main(){

    sem_t *semaphore;
    sem_t *semaphore2;

    int n=0;


    if ((semaphore = sem_open("ex8semA", O_CREAT, 0644,1)) == SEM_FAILED) 
    {
        perror("Error creating semaphore!");
        exit(EXIT_FAILURE);
    }

    if ((semaphore2 = sem_open("ex8semA", O_CREAT, 0644,1)) == SEM_FAILED) 
    {
        perror("Error creating semaphore!");
        exit(EXIT_FAILURE);
    }


    pid_t p;

    p = fork();

    if(p>0){
        while(n<NLOOPS){
            sem_wait(semaphore);
            printf("S");
            fflush(stdout);
            sem_post(semaphore2);
            n++;
        } 
    } else {
        while(n<NLOOPS){
            sem_wait(semaphore2);
            printf("C");
            fflush(stdout);
            sem_post(semaphore);

            n++;
        }
    }

    sem_close(semaphore);
    sem_close(semaphore2);


    if(p>0){
        wait(NULL);
    } else {
        exit(EXIT_SUCCESS); //when a process dies, voluntarily or unvoluntarily, the sem_close operation occurs automatically
    }

    
    sem_unlink("ex8semA");
    sem_unlink("ex8semB");

    return EXIT_SUCCESS;
}