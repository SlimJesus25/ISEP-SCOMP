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

#define MUTEX_QUANT 3
#define SYNCS_QUANT 2
#define MUTEX_GENERIC_NAME "/mutex"
#define SYNC_GENERIC_NAME "/sync"
#define WRITERS 5
#define READERS 5
#define TOTAL WRITERS + READERS
#define SHARED_MEMORY_NAME "/shared_memory"
#define SHARED_MEMORY_SIZE WRITERS
#define READ 0
#define WRITE 1

typedef struct{
    int writers;
    int readers;
    char txt[64];
}info_t;

/*
        ## Explicação ##
    Este exercício foi resolvido com base na solução do "Problema dos Leitores e Escritores".
    Como tal, foram utilizados 5 semáforos de exclusão mútua, onde:
    O mutexes[0] vai controlar o acesso à zona crítica (memória partilhada) para os leitores.
    O mutexes[1] vai controlar também o acesso à zona crítica, mas para os escritores.
    O mutexes[2] vai controlar o acesso ao semáforo syncs[READ], ou seja, permite que apenas um processo leitor entre nesta zona.
    O syncs[READ] vai controlar a entrada de leitores nas zonas críticas.
    O syncs[WRITE] vai controlar a entrada de escritores nas zonas críticas.
*/

int main(int argc, char *argv[]){

    sem_t* mutexes[MUTEX_QUANT];
    create_semaphores(mutexes, MUTEX_QUANT, MUTEX_GENERIC_NAME, MUTEX_INITAL_VALUE);

    sem_t* syncs[SYNCS_QUANT];
    create_semaphores(syncs, SYNCS_QUANT,SYNC_GENERIC_NAME, MUTEX_INITAL_VALUE);

    int fd, size = SHARED_MEMORY_SIZE*sizeof(info_t);
    info_t *addr;

    fd = shm_open(SHARED_MEMORY_NAME, O_CREAT|O_EXCL|O_RDWR, S_IRUSR|S_IWUSR);
    if(fd < 0){
        perror("shm_open");
        exit(EXIT_FAILURE);
    }

    if(ftruncate(fd, size) < 0){
        perror("ftruncate");
        exit(EXIT_FAILURE);
    }

    addr = (info_t*)mmap(NULL, size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
    if(addr == MAP_FAILED){
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    int i;
    for(i=0;i<TOTAL;i++){
        pid_t p = fork();
        if(p < 0){
            perror("fork");
            exit(EXIT_FAILURE);
        }
        if(p == 0){
            break;
        }
    }

    // Writer.
    if(i < WRITERS){

        // Mutex para incrementar os escritores em atividade (na ocasião deste ser o primeiro escritor, vai bloquear até que não estejam leitores em atividade).
        sem_wait(mutexes[1]);
        addr->writers += 1;
        if((addr+i)->writers == 1){
            sem_wait(syncs[READ]);
        }
        sem_post(mutexes[1]);

        // Assegura a exclusividade para escrever o respetivo PID e a hora do sistema na zona de memória partilhada.
        sem_wait(syncs[WRITE]);

        time_t rawtime;
        struct tm * timeinfo;

        time(&rawtime);
        timeinfo = localtime(&rawtime);

        snprintf((addr+i)->txt, 35, "%d - %s\n", getpid(), asctime(timeinfo));

        printf("Actual readers: %d\nActual writers: %d\n\n", addr->readers, addr->writers);

        sem_post(syncs[WRITE]);

        // Mutex para desincrementar os escritores em atividade (na ocasião deste ser o último escritor, vai desbloquear os leitores para entrarem em atividade).
        sem_wait(mutexes[1]);
        addr->writers -= 1;
        if(addr->writers == 0){
            sem_post(syncs[READ]);
        }
        sem_post(mutexes[1]);
        exit(EXIT_SUCCESS);
    }
    // Reader.
    else if(i < TOTAL){

        // Mutex para acesso a zona crítica.
        sem_wait(mutexes[2]);

        // Só vai conseguir aceder a esta zona se não existir nenhum escritor em atividade ou um leitor a incrementar o número atual de leitores (próxima fase).
        sem_wait(syncs[READ]);

        // Mutex para inrementar o número de escritores em atividade (na ocasião deste ser o primeiro leitor, vai bloquear até não haver escritores em atividade).
        sem_wait(mutexes[0]);
        addr->readers += 1;
        if(addr->readers == 1){
            sem_wait(syncs[WRITE]);
        }
        sem_post(mutexes[0]);

        // Permite a entrada de novos leitores.
        sem_post(syncs[READ]);

        // Permite a entrada de novos escritores a esta zona crítica.
        sem_post(mutexes[2]);

        sleep(1);
        printf("Actual readers: %d\nString on shared memory: %s\n\n", addr->readers, (addr+(i-WRITERS))->txt);

        sem_wait(mutexes[0]);
        addr->readers -= 1;
        if(addr->readers == 0){
            sem_post(syncs[WRITE]);
        }
        sem_post(mutexes[0]);
        exit(EXIT_SUCCESS);
    }

    for(int i=0;i<TOTAL;i++){
        wait(NULL);
    }

    unlink_semaphores(MUTEX_GENERIC_NAME, MUTEX_QUANT);
    unlink_semaphores(SYNC_GENERIC_NAME, SYNCS_QUANT);

    if(munmap(addr, size) < 0){
        perror("munmap");
        exit(EXIT_FAILURE);
    }

    if(close(fd) < 0){
        perror("close");
        exit(EXIT_FAILURE);
    }

    if(shm_unlink(SHARED_MEMORY_NAME) < 0){
        perror("shm_unlink");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}