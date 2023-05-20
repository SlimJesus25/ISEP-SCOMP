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

#define SEMAPHORE_1_INITAL_VALUE 0
#define SEMAPHORE_2_INITAL_VALUE 2
#define SEMAPHORE_NAME_1 "/barrier_ex9"
#define SEMAPHORE_NAME_2 "/lim_res_ex9"
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

    sem_t* barrier_1 = sem_open(SEMAPHORE_NAME_1, O_CREAT, S_IRUSR|S_IWUSR, SEMAPHORE_1_INITAL_VALUE);
    sem_t* barrier_2 = sem_open(SEMAPHORE_NAME_2, O_CREAT, S_IRUSR|S_IWUSR, SEMAPHORE_2_INITAL_VALUE);

    if(barrier_1 == SEM_FAILED || barrier_2 == SEM_FAILED){
        perror("sem_open");
        exit(EXIT_FAILURE);
    }

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
        sem_post(barrier_1);
        
        // Aguarda que o outro processo abra caminho.
        sem_wait(barrier_2);

        eat_and_drink();
        exit(EXIT_SUCCESS);
        
    }else{
        srand(getpid());

        sleep(rand() % 5);

        buy_chips();
        
        sem_post(barrier_2);

        sem_wait(barrier_1);

        eat_and_drink();
    }

    wait(NULL);

    if(sem_unlink(SEMAPHORE_NAME_1) < 0 || sem_unlink(SEMAPHORE_NAME_2) < 0){
        perror("sem_unlink");
    }

    exit(EXIT_SUCCESS);
}