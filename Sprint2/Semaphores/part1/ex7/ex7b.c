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
#include "../../semaphore_utils.h"
#define SEMAPHORES 10
#define SEMAPHORE_INITAL_VALUE 0
#define SEMAPHORE_GENERIC_NAME "/sem_ex9_"
#define CHILDREN 9

/*
        ## Explicação ##
    A solução para este exercício foi inspirado numa sincronização entre processos.
    Onde cada processo desbloqueia SEMAPHORES vezes o seu semáforo de atribuição.
    É importante referir que existem SEMAPHORES semáforos com o valor inicial a zero onde vão ser incrementados CHILDREN vezes.
*/

void buy_chips()
{

    char txt[] = {"Buying chips...\n"};

    write(STDOUT_FILENO, txt, strlen(txt) + 1);
}

void buy_beer()
{

    char txt[] = {"Buying beer...\n"};

    write(STDOUT_FILENO, txt, strlen(txt) + 1);
}

void eat_and_drink()
{

    char txt[] = {"Eating and drinking...\n"};

    write(STDOUT_FILENO, txt, strlen(txt) + 1);
}

void task(int i, sem_t * semaphores[]){
    srand(getpid());

    // Dorme por x segundos.
    sleep(rand() % 5);

    // Ou compra cerveja ou compra batatas.
    if (rand() % 2 == 0){
        buy_beer();
    }else{
        buy_chips();
    }

    // Incrementa o semáforo relativo a este processo SEMAPHORES vezes.
    for (int j = 0; j < SEMAPHORES; j++){
        sem_post(semaphores[i]);
    }

    // Aguarda por todos os incrementos dos restantes processos.
    for (int j = 0; j < SEMAPHORES; j++){
        sem_wait(semaphores[j]);
    }

    // Toda a gente já comprou alguma coisa e agora vão comer ou beber.
    eat_and_drink();
}

int main(int argc, char *argv[])
{

    sem_t *semaphores[SEMAPHORES];

    create_semaphores(semaphores, SEMAPHORES, SEMAPHORE_GENERIC_NAME, SYNCHRONIZATION_INITAL_VALUE);

    for (int i = 0; i < CHILDREN; i++){
        pid_t p = fork();

        if (p < 0){
            perror("fork");
            exit(EXIT_FAILURE);
        }

        // Processos filho.
        if (p == 0){
            task(i, semaphores);
            exit(EXIT_SUCCESS);
        }
    }

    task(SEMAPHORES-1, semaphores);

    for(int i = 0; i < CHILDREN; i++){
        wait(NULL);
    }

    unlink_semaphores(SEMAPHORE_GENERIC_NAME, SEMAPHORES);

    exit(EXIT_SUCCESS);
}