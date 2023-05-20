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

#define MUTEX_NAMES "/e11_mutex_"
#define SYNC_NAMES "/e11_sync_arr_"
#define RESOURCE_NAME "/e11_sync_"

#define TRAIN_CAPACITY 15
#define TRAIN_DOOR 3
#define PASSENGER 35
#define ENTERING_TRAIN 10
#define LEAVING_TRAIN 10
#define STAYING_TRAIN 5
#define DOOR_ONE 0
#define DOOR_TWO 1
#define DOOR_THREE 2

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Usar flag -lm para log10                                                      //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int access_train_door(sem_t* sem[]){
    while(1 == 1){
        if(sem_trywait(sem[DOOR_ONE]) == 0){
            return DOOR_ONE;
        }
        if(sem_trywait(sem[DOOR_TWO]) == 0){
            return DOOR_TWO;
        }
        if(sem_trywait(sem[DOOR_THREE]) == 0){
            return DOOR_THREE;
        }
    }
}

/**
 * Para solucionar este problema foram implementados 3 tipos de semáforos, exclusão mútua, acesso a recursos partilhados e sincronização. Utilizou-se:
 *  - Um vetor de semáforos de tamanho DOORS para exclusão mútua, onde serve para garantir que o processo de entrar/sair por uma porta é feito por um e um só processo.
 *  - Um vetor de semáforos de tamanho PASSENGER que vai assegurar que todos os processos vão iniciar o "core" ao mesmo tempo (isto é opcional, serve para não enviesar os dados).
 *  - Um semáforo que vai controlar os recursos (espaço no comboio). Na ocasião de não haver, bloqueia processos que estejam a tentar entrar.
*/

int main(){

    if(LEAVING_TRAIN + STAYING_TRAIN > TRAIN_CAPACITY){
        printf("train capacity invalid");
        exit(EXIT_FAILURE);
    }
    
    // Este semáforo vai permitir a quem quer entrar saber se há espaço ou não.
    sem_t *shared_resources;
    shared_resources = sem_open(RESOURCE_NAME, O_CREAT, S_IRUSR|S_IWUSR, TRAIN_CAPACITY - LEAVING_TRAIN + STAYING_TRAIN);
    if(shared_resources == SEM_FAILED){
        perror("sem_open");
        exit(EXIT_FAILURE);
    }
    
    // Este semáforo é de exclusão mútua para cada uma das portas e vai assegurar que apenas uma pessoa entra/sai de cada vez.
    sem_t *mutex_semaphores[TRAIN_DOOR];
    create_semaphores(mutex_semaphores, TRAIN_DOOR, MUTEX_NAMES, MUTEX_INITAL_VALUE);
    
    // Este semáforo vai sincronizar todos os processos criados para o programa iniciar com todos os processos no mesmo ponto de partida.
    sem_t *sync_semaphores[PASSENGER];
    create_semaphores(sync_semaphores, PASSENGER, SYNC_NAMES, SYNCHRONIZATION_INITAL_VALUE);
    
    printf("The train capacity is %d, has %d doors and there are:\n %d people trying to enter;\n %d people staying;\n %d people trying to leave.\n\n",
     TRAIN_CAPACITY, TRAIN_DOOR, ENTERING_TRAIN, STAYING_TRAIN, LEAVING_TRAIN);

    printf("\t ### TRAIN PROGRAM STARTING UP...###\n\n\n");
    
    int creation_id;
    pid_t p;
    for(creation_id=0;creation_id<PASSENGER;creation_id++){
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

        // Incrementa PASSENGER vezes o semáforo relativo a este processo.
        for(int i=0;i<PASSENGER;i++){
            sem_post(sync_semaphores[creation_id]);
        }
    
        // Sincroniza os processos para todos começarem ao mesmo tempo.
        for(int i=0;i<PASSENGER;i++){
            sem_wait(sync_semaphores[i]);
        }
        
        // Processos que estão a tentar entrar no comboio.
        if(creation_id >= 0 && creation_id < ENTERING_TRAIN){
            sem_wait(shared_resources);
            int left_door = access_train_door(mutex_semaphores);
            printf("%d is ENTERING by the door %d...\n", getpid(), left_door);

            // Demora 2 segundos a entrar pela porta...
            sleep(2);
    
            // Chegando aqui, significa que o passageiro entrou no comboio.
            sem_post(mutex_semaphores[left_door]);

            printf("%d ENTERED by door %d\n", getpid(), left_door);
            exit(EXIT_SUCCESS);
        }
    
        // Processos que estão a tentar sair do comboio.
        if(creation_id >= ENTERING_TRAIN && creation_id < ENTERING_TRAIN+LEAVING_TRAIN){
    
            // Tenta sair por uma das três portas até conseguir.
            int left_door = access_train_door(mutex_semaphores);
            printf("%d is LEFTING by the door %d...\n", getpid(), left_door);

            // Demora 2 segundos a sair pela porta...
            sleep(2);
    
            // Chegando aqui, significa que o passageiro saiu do comboio, por isso vai libertar uma posição/lugar para outro.
            sem_post(mutex_semaphores[left_door]);
            sem_post(shared_resources);
            printf("%d LEFT by door %d\n", getpid(), left_door);
            exit(EXIT_SUCCESS);
        }
    }

    for(int i=0;i<PASSENGER;i++){
        wait(NULL);
    }
    
    if(sem_unlink(RESOURCE_NAME) < 0){
        perror("sem_unlink");
        exit(EXIT_FAILURE);
    }
    
    unlink_semaphores(MUTEX_NAMES, TRAIN_DOOR);
    unlink_semaphores(SYNC_NAMES, PASSENGER);
    

    exit(EXIT_SUCCESS);
}