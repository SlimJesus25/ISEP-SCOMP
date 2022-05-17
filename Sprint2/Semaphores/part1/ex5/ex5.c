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
#include <math.h>

int main(){

    sem_t *sem; // Semaphore pointer.

    // Creates semaphore and tests it is valid.
    if((sem = sem_open("ex5sem", O_CREAT, 0644, 0)) == SEM_FAILED){
        perror("Error creating semaphore!");
        exit(EXIT_FAILURE);
    }

    if(fork() == 0){
        printf("I'm the child\n");
        sem_post(sem);
    }else{
        sem_wait(sem);
        printf("I'm the father\n");
    }

    sem_close(sem);
    sem_unlink("ex5sem");

    return EXIT_SUCCESS;
}