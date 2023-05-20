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
#include "../semaphore_utils.h"

#define PRODUCER_ID 0
#define CONSUMER_ID 1
#define PRODUCERS 2
#define PRODUCERS_MAX_LIMIT 30
#define SHARED_MEMORY_SIZE 10
#define CONSUMERS 1
#define SYNC2_NAME "/ex13sync2"
#define MUTEX_NAME "/ex13mutex"
#define SHARED_NAME "/ex13shrdmem"
#define LIMITED_RESOURCES_NAME "/ex13lr"
#define SYNC_NAME "/ex13sync"
#define LIMITED_RESOURCES_INITAL_VALUE SHARED_MEMORY_SIZE

/**
 * Para resolver este problema, vai ser utilizado um semáforo de exclusão mútua, um de sincronização e outro de acesso a recursos limitados.
 *  - O semáforo de exclusão mútua vai assegurar que apenas um processo acede à secção crítica.
 *  - O de sincronização assegura que existem dados novos para o consumidor consumir.
 *  - E o de acesso a recursos limitados para que os produtores bloqueem, caso o buffer esteja cheio.
 *  
*/
int main(){

    pid_t p;

    sem_t* sync = sem_open(SYNC_NAME, O_CREAT, S_IRUSR|S_IWUSR, SYNCHRONIZATION_INITAL_VALUE);
    sem_t* mutex = sem_open(MUTEX_NAME, O_CREAT, S_IRUSR|S_IWUSR, MUTEX_INITAL_VALUE);
    sem_t* limited_resources = sem_open(LIMITED_RESOURCES_NAME, O_CREAT, S_IRUSR|S_IWUSR, LIMITED_RESOURCES_INITAL_VALUE);

    if(sync == SEM_FAILED || mutex == SEM_FAILED || limited_resources == SEM_FAILED){
        perror("sem_open");
        exit(EXIT_FAILURE);
    }

    int fd, size = SHARED_MEMORY_SIZE*sizeof(int);
    int *addr;
    int r;

    fd = shm_open(SHARED_NAME, O_CREAT|O_RDWR, S_IRUSR|S_IWUSR);
    if(fd < 0){
        perror("shm_open");
        exit(EXIT_FAILURE);
    }

    if(ftruncate(fd, size) < 0){
        perror("ftruncate");
        exit(EXIT_FAILURE);
    }

    addr = (int*)mmap(NULL, size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
    if(addr == MAP_FAILED){
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    // Criação de PRODUCERS produtores.
    for(int i=0;i<PRODUCERS;i++){
        p = fork();
        if(p == 0){
            break;
        }
    }

    if(p >= CONSUMER_ID){
        for(int i=0;i<PRODUCERS_MAX_LIMIT*PRODUCERS;i++){

            // Aguarda pela notificação do produtor da existência de valores no buffer.
            // O consumidor bloqueia, se não existir informação nova no buffer.
            sem_wait(sync);
            
            // Ganha acesso exclusivo à zona crítica.
            sem_wait(mutex);

            int v = *(addr+(i%SHARED_MEMORY_SIZE));
            printf("Value received: %d\n", v);

            // Liberta o acesso exclusivo.
            sem_post(mutex);
            
            // Dá a vaga de uma posição do buffer.
            sem_post(limited_resources);
        }
    }

    if(p == PRODUCER_ID){
        srand(getpid());
        for(int i=0;i<PRODUCERS_MAX_LIMIT;i++){

            // Na ocasião de o buffer estar cheio, o produtor vai ficar bloqueado.
            sem_wait(limited_resources);

            // A partir do momento que entram aqui, estão a entrar numa zona de crítica.
            sem_wait(mutex);

            r = rand() % 100;

            *(addr+(i%SHARED_MEMORY_SIZE)) = r;

            // Desbloqueia a zona crítica.
            sem_post(mutex);

            // Indica ao produtor que tem mais um valor no buffer.
            sem_post(sync);
        }
        exit(EXIT_SUCCESS);
    }

    if(munmap(addr, size) < 0){
        perror("munmap");
        exit(EXIT_FAILURE);
    }

    /*
        Fecha o descritor de ficheiros retornado pelo shm_open().
            int fd -> Descritor de ficheiros delvolvido pelo shm_open().
    */
    if(close(fd) < 0){
        perror("close");
        exit(EXIT_FAILURE);
    }

    /*
        Remove a zona de memória partilhada do sistema de ficheiros. Marca-a para ser apagada, mal todos os processos que, eventualmente, a estejam a usar, fechem.
        Nota: Não é instantâneo! Se algum processo estiver a usar a zona de memória, a memória vai ser marcada para ser apagada, mal consiga!
            const char* name -> Nome da zona de memória partilhada.
    */
    if(shm_unlink(SHARED_NAME) < 0){
        perror("shm_unlink");
        exit(EXIT_FAILURE);
    }

    if(sem_unlink(MUTEX_NAME) < 0 || sem_unlink(LIMITED_RESOURCES_NAME) < 0 || sem_unlink(SYNC_NAME) < 0){
        perror("sem_unlink");
        exit(EXIT_FAILURE);
    }


    exit(EXIT_SUCCESS);
}