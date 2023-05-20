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
#include "../../../semaphore_utils.h"


#define MUTEX_GENERIC_NAME "/mutex"
#define LIMITED_RES_GENERIC_NAME "/lim_res"
#define SYNC_GENERIC_NAME "/sync"
#define CLIENT_SEM_NAME "/client"
#define CLUB_SEM_NAME "/club"
#define SHARED_MEMORY_NAME "/shared_memory"
#define CLUB_CAPACITY 10
#define CLIENTS 20

/*
        ## Explicação ##
        Para resolver este problema, a solução foi inspirada no Sleeping Barber.
    
*/

int main(int argc, char *argv[]){

    // Indica a chegada de um cliente.
    sem_t* client = sem_open(CLIENT_SEM_NAME, O_CREAT, 0644, SYNCHRONIZATION_INITAL_VALUE);

    // Indica se o clube está cheio.
    sem_t* club = sem_open(CLUB_SEM_NAME, O_CREAT, 0644, SYNCHRONIZATION_INITAL_VALUE);

    // Exclusão mútua para secção crítica.
    sem_t* mutex = sem_open(MUTEX_GENERIC_NAME, O_CREAT, 0644, MUTEX_INITAL_VALUE);

    // Recursos limitados (bar).
    sem_t* lim_res = sem_open(LIMITED_RES_GENERIC_NAME, O_CREAT, 0644, CLUB_CAPACITY);

    if(client == SEM_FAILED || club == SEM_FAILED || mutex == SEM_FAILED || lim_res == SEM_FAILED){
        perror("sem_open");
        exit(EXIT_FAILURE);
    }

    int fd, size = sizeof(int);
    int *addr;

    fd = shm_open(SHARED_MEMORY_NAME, O_CREAT|O_EXCL|O_RDWR, S_IRUSR|S_IWUSR);
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

    // Atribuição do espaço livre do club na memória partilhada.
    *(addr) = CLUB_CAPACITY;

    // Criação de clientes.
    int i;
    for(i=0;i<CLIENTS;i++){
        pid_t p = fork();
        if(p < 0){
            perror("fork");
            exit(EXIT_FAILURE);
        }
        if(p == 0){
            break;
        }
    }

    if(i == CLIENTS){
        int iteration = 0;
        do{

            // Enquanto não chega nenhum cliente, o bar está numa espera passiva.
            sem_wait(client);

            printf("The security noticed that a client didn't order anything...\n");

            // Após x segundos, o cliente é expulso do bar por estar apenas a olhar e não consumir.
            sleep(4);

            // Indica que o bar já dispensou um cliente e agora vai atentar noutro.
            sem_post(club);
            
        }while(++iteration < CLIENTS);
    }

    if(i < CLIENTS){
        
        // Caso haja espaço, vai ser possível desincrementar. Caso contrário, aguarda.
        sem_wait(lim_res);

        // Acede exclusivamente à memória partilhada para atualizar o número de clientes no bar.
        sem_wait(mutex);
        *(addr) -= 1;
        sem_post(mutex);
        

        // Indica ao bar que vai entrar.
        sem_post(client);

        printf("Client %d entered the club...\n", getpid());

        // Aguarda pela atenção do bar (para ser expulso).
        sem_wait(club);

        // É expulso do bar e atualiza o espaço livre.
        sem_wait(mutex);
        *(addr) += 1;
        sem_post(lim_res);
        printf("Client %d was kicked out of the club because he was not consuming anything...\n\n\n", getpid());
        sem_post(mutex);
        
        exit(EXIT_SUCCESS);
    }
    
    for(int i=0;i<CLIENTS;i++){
        wait(NULL);
    }

    if(sem_unlink(CLIENT_SEM_NAME) < 0 || sem_unlink(CLUB_SEM_NAME) < 0 || sem_unlink(MUTEX_GENERIC_NAME) < 0 || sem_unlink(LIMITED_RES_GENERIC_NAME)){
        perror("sem_unlink");
        exit(EXIT_FAILURE);
    }

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