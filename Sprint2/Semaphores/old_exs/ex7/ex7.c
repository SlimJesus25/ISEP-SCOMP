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
#define SEMAPHORE_NAME "/exercicio7"
#define SEMAPHORE_QUANTITY 3
#define CHILDREN 3


/*
        ## Explicação ##
    A estratégia utilizada para resolver este exercício foi utilizar 3 semáforos (mínimos possíveis) de 
    sincronização entre processos (valor inicial igual a zero).
*/

int main(int argc, char *argv[]){

    printf("\n");

    char sem_name[SEMAPHORE_QUANTITY][30] = {"/exercicio7_1", "/exercicio7_2", "/exercício7_3"};

    char info[6][25] = {"Sistemas ", "de", " Computadores - ",
     "a ", "melhor ", "disciplina!"};

    sem_t* proc_sync_1 = sem_open(sem_name[0], O_CREAT, S_IRUSR|S_IWUSR, SEMAPHORE_INITAL_VALUE);
    sem_t* proc_sync_2 = sem_open(sem_name[1], O_CREAT, S_IRUSR|S_IWUSR, SEMAPHORE_INITAL_VALUE);
    sem_t* proc_sync_3 = sem_open(sem_name[2], O_CREAT, S_IRUSR|S_IWUSR, SEMAPHORE_INITAL_VALUE);

    if(proc_sync_1 == SEM_FAILED || proc_sync_2 == SEM_FAILED || proc_sync_3 == SEM_FAILED){
        perror("sem_open");
        exit(EXIT_FAILURE);
    }

    for(int i=0;i<CHILDREN;i++){
        pid_t p = fork();
        if(p < 0){
            perror("fork");
            exit(EXIT_FAILURE);
        }

        if(p == 0){
            if(i == 0){
                sem_wait(proc_sync_1);
                write(STDOUT_FILENO, info[i], strlen(info[i])+1);
                sem_post(proc_sync_2);
            
                sem_wait(proc_sync_1);
                write(STDOUT_FILENO, info[i+3], strlen(info[i+3])+1);
                sem_post(proc_sync_2);
            }else if(i == 1){
                sem_wait(proc_sync_2);
                write(STDOUT_FILENO, info[i], strlen(info[i])+1);
                sem_post(proc_sync_3);
            
                sem_wait(proc_sync_2);
                write(STDOUT_FILENO, info[i+3], strlen(info[i+3])+1);
                sem_post(proc_sync_3);
            }else{
                sem_wait(proc_sync_3);
                write(STDOUT_FILENO, info[i], strlen(info[i])+1);
                sem_post(proc_sync_1);
            
                sem_wait(proc_sync_3);
                write(STDOUT_FILENO, info[i+3], strlen(info[i+3])+1);
                sem_post(proc_sync_1);
            }
            exit(EXIT_SUCCESS);
        }
    }

    sem_post(proc_sync_1);

    for(int i=0;i<CHILDREN;i++){
        wait(NULL);
    }
    printf("\n");

    if(sem_unlink(sem_name[0]) < 0 || sem_unlink(sem_name[1]) < 0 || sem_unlink(sem_name[2]) < 0){
        perror("sem_unlink");
    }

    exit(EXIT_SUCCESS);
}