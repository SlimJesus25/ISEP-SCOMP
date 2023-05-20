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
#include "info.h"

#define CHILDREN 10


/*
        ## Explicação ##
    Foram utilizados 2 tipos de semáforos um de exclusão mútua e outro sincronização de processos.
    A exclusão mútua serve para garantir que apenas um processo lia/escrevia na memória partilhada.
    A sincronização é para colocar todos os processos no mesmo ponto de partida e assim evita resultados enviesados.
    
*/

int main(int argc, char *argv[]){

    srand(time(NULL));

    // Semáforo de exclusão mútua para aceder à zona de memória partilhada.
    sem_t* mutex = sem_open(MUTEX_NAME, O_CREAT, S_IRUSR|S_IWUSR, MUTEX_INITAL_VALUE);

    // Semáforo "barrier" para obrigar todos os processos a ter o mesmo ponto de partida.
    // Caso contrário, a memória partilhada ia ser preenchida, quase sempre, por apenas 1 processo.
    sem_t* barrier[CHILDREN];

    char* sem_name = (char*)malloc(10*sizeof(char));

    for(int i=0;i<CHILDREN;i++){
        int int_size;
        if(i == 0){
            int_size = 1;
        }else{
        // Variável vai ficar com o número de dígitos da iteração.
            int_size = log10(i)+1;
        }

        int str_len = strlen(BARRIER_NAME)+1+int_size;
        
        sem_name = (char*)realloc(sem_name, str_len*sizeof(char));
        snprintf(sem_name, str_len, "%s%d", BARRIER_NAME, i);
        barrier[i] = sem_open(BARRIER_NAME, O_CREAT, S_IRUSR|S_IWUSR, SYNC_INITAL_VALUE);
        if(barrier[i] == SEM_FAILED){
            perror("sem_open");
            exit(EXIT_FAILURE);
        }
    }

    int fd, size = sizeof(info_t);
    info_t *addr;

    if(mutex == SEM_FAILED){
        perror("sem_open");
        exit(EXIT_FAILURE);
    }

    // Cria e abre uma zona de memória partilhada.
    fd = shm_open(SHARED_MEMORY_NAME, O_CREAT|O_RDWR, S_IRUSR|S_IWUSR);
    if(fd < 0){
        perror("shm_open");
        exit(EXIT_FAILURE);
    }

    // Define o tamanho da área e inicializa a zero.
    if(ftruncate(fd, size) < 0){
        perror("ftruncate");
        exit(EXIT_FAILURE);
    }

    // Mapeia a zona de memória partilhada no espaço do processo.
    addr = (info_t*)mmap(NULL, size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
    if(addr == MAP_FAILED){
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    pid_t p;
    int id = 0;
    for(int id=0;id<CHILDREN;id++){
        p = fork();
        if(p < 0){
            perror("fork");
            exit(EXIT_FAILURE);
        }
        
        if(p == 0){
            break;
        }
    }

    if(p == 0){
        for(int i=0;i<CHILDREN;i++){
            sem_post(barrier[id]);
        }

        for(int i=0;i<CHILDREN;i++){
            sem_wait(barrier[i]);
        }

        if(p == 0){
            while(0 == 0){
                sem_wait(mutex);

                // Na ocasião de já estar cheio, vai incrementar o semáforo (para não bloquear outros processos) e sair do loop. 
                if(addr->iteration == STRING_QUANTITY){
                    sem_post(mutex);
                    break;
                }

                char txt[STRING_CHARACTER_NUMBER];
                snprintf(txt, STRING_CHARACTER_NUMBER, "I'm the Father - with PID %d", getpid());

                strcpy(addr->array_strings[addr->iteration], txt);

                addr->iteration = addr->iteration + 1;

                sem_post(mutex);
                sleep(1);
            }
            exit(EXIT_SUCCESS);
        }
    }

    for(int i=0;i<CHILDREN;i++){
        wait(NULL);
    }

    // Disconecta a zona de memória partilhada do espaço do processo.
    if(munmap(addr, size) < 0){
        perror("munmap");
        exit(EXIT_FAILURE);
    }

    // Fecha o descritor de ficheiros retornado pelo shm_open().
    if(close(fd) < 0){
        perror("close");
        exit(EXIT_FAILURE);
    }
    
    for(int i=0;i<CHILDREN;i++){
        int int_size;
        if(i == 0){
            int_size = 1;
        }else{
            int_size = log10(i)+1;
        }

        int str_len = strlen(BARRIER_NAME)+1+int_size;
        
        char sem_name[str_len];
        snprintf(sem_name, str_len, "%s%d", BARRIER_NAME, i);

        if(sem_unlink(sem_name) < 0){
            perror("sem_unlink");
            exit(EXIT_FAILURE);
        }
    }

    if(sem_unlink(MUTEX_NAME) < 0){
        perror("sem_unlink");
        exit(EXIT_FAILURE);
    }
    
    exit(EXIT_SUCCESS);
}