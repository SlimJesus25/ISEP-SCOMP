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

#define SEMAPHORE_INITAL_VALUE 0
#define SEMAPHORE_NAME "/exercicio5"
#define CHILDREN 10


/*
        ## Explicação ##
    Para resolver este exercício foi utilizado um semáforo de sincronização de processos, ou seja, com o valor inicial igual a zero.
    O pai tem que esperar que o semáforo tenha uma "transição positiva" para conseguir aceder à sua secção crítica e apenas o filho é
    capaz de incrementar o semáforo.
*/

int main(int argc, char *argv[]){

    sem_t* proc_sync = sem_open(SEMAPHORE_NAME, O_CREAT, S_IRUSR|S_IWUSR, SEMAPHORE_INITAL_VALUE);

    if(proc_sync == SEM_FAILED){
        perror("sem_open");
        exit(EXIT_FAILURE);
    }
    pid_t p = fork();
    if(p < 0){
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if(p == 0){
        printf("I'm the child!\n");
        sem_post(proc_sync);
        exit(EXIT_SUCCESS);
    }else{
        sem_wait(proc_sync);
        printf("I'm the father!\n");
        sem_post(proc_sync);
    }

    if(sem_unlink(SEMAPHORE_NAME) < 0){
        perror("sem_unlink");
    }

    exit(EXIT_SUCCESS);
}