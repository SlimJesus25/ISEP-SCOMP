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

#define BARRIERS 2
#define SEMAPHORE_NAME "/barrier"
#define CHILDREN 3


/*
        ## Explicação ##
    A solução para este exercício foi inspirado numa Sincronização entre processos.
    Onde cada processo desbloqueia o outro.
*/

void buy_chips(){

    char txt[] = {"Buying chips...\n"};

    write(STDOUT_FILENO, txt, strlen(txt)+1);
}

void buy_beer(){

    char txt[] = {"Buying beer...\n"};

    write(STDOUT_FILENO, txt, strlen(txt)+1);
}

void eat_and_drink(){

    char txt[] = {"Eating and drinking...\n"};

    write(STDOUT_FILENO, txt, strlen(txt)+1);
}

int main(int argc, char *argv[]){

    sem_t* barriers[BARRIERS];

    create_semaphores(barriers, BARRIERS, SEMAPHORE_NAME, SYNCHRONIZATION_INITAL_VALUE);

    pid_t p = fork();

    if(p < 0){
        perror("fork");
        exit(EXIT_FAILURE);
    }

    // Processo 2
    if(p == 0){
        srand(getpid());
        
        // Dorme por x segundos.
        sleep(rand() % 5);

        // Compra cerveja.
        buy_beer();

        // Abre caminho para o outro processo.
        sem_post(barriers[0]);
        
        // Aguarda que o outro processo abra caminho.
        sem_wait(barriers[1]);

        eat_and_drink();
        exit(EXIT_SUCCESS);
        
    }else{
        srand(getpid());

        sleep(rand() % 5);

        buy_chips();
        
        sem_post(barriers[1]);

        sem_wait(barriers[0]);

        eat_and_drink();
    }

    wait(NULL);

    unlink_semaphores(SEMAPHORE_NAME, BARRIERS);

    exit(EXIT_SUCCESS);
}