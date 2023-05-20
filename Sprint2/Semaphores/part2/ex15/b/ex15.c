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
#define BARRIER_GENERIC_NAME "/barrier"
#define SYNC_GENERIC_NAME "/sync"
#define CLIENT_SEM_NAME "/client"
#define CLUB_SEM_NAME "/club"
#define SHARED_MEMORY_NAME "/shared_memory"
#define CLUB_CAPACITY 10
#define CLIENTS 20
#define VIPS 3
#define SPECIALS 7
#define NORMALS 10
#define CLIENT_TYPES 3

typedef struct{
    int free_space;
    int created_clients;
}info_t;

/*
        ## Explicação ##
        Para resolver este problema, a solução foi inspirada no Sleeping Barber.
    
*/


/**
 * Esta função vai preencher o array com valores entre 0 e 2 e dar shuffle.
*/
void fill_array_with_client_types(int array[], int vip_quant, int special_quant, int normal_quant){

    int length = vip_quant + special_quant + normal_quant;
    int vip_c, special_c, normal_c;
    vip_c = special_c = normal_c = 0;

    for(int i=0;i<length;i++){
        if(vip_c < vip_quant){
            array[i] = 0;
            vip_c++; 
        }else if(special_c < special_quant){
            array[i] = 1;
            special_c++;
        }else{
            array[i] = 2;
            normal_c++;
        }
    }

    for(int i=0;i<length;i++){
        int aux, r = rand() % length;
        aux = array[i];
        array[i] = array[r];
        array[r] = aux;
    }
}

/**
 * Vai preencher o array txt com o seu respetivo valor textual e retornar o respetivo valor numérico (0 para VIP, 1 para Special, etc...).
*/
int get_client_type(char txt[], int array[], int i){

    if(array == NULL){
        if(i == 0){
            snprintf(txt, 4, "VIP");
        }else if(i == 1){
            snprintf(txt, 8, "Special");
        }else{
            snprintf(txt, 7, "Normal");
        }
        return -1;
    }

    if(array[i] == 0){
        snprintf(txt, 4, "VIP");
    }else if(array[i] == 1){
        snprintf(txt, 8, "Special");
    }else{
        snprintf(txt, 7, "Normal");
    }
    return array[i];
}

int main(int argc, char *argv[]){

    srand(time(NULL));
    
    if(VIPS+NORMALS+SPECIALS != CLIENTS){
        printf("Macro error!");
        exit(EXIT_FAILURE);
    }

    // Array que vai permitir atribuir um tipo de cliente a cada um dos processos.
    int array[CLIENTS];
    fill_array_with_client_types(array, VIPS, SPECIALS, NORMALS);

    // Array de semáforos para cada tipo de cliente (útil para dar entrada aos mais prioritários).
    sem_t* client_type[CLIENT_TYPES];
    create_semaphores(client_type, CLIENT_TYPES, CLIENT_SEM_NAME, SYNCHRONIZATION_INITAL_VALUE);

    // Barreira para que o core do programa seja alcançado pelos processos após todos serem criados.
    sem_t* barrier = sem_open(BARRIER_GENERIC_NAME, O_CREAT, 0644, SYNCHRONIZATION_INITAL_VALUE);

    // Indica se o clube está cheio.
    sem_t* club = sem_open(CLUB_SEM_NAME, O_CREAT, 0644, SYNCHRONIZATION_INITAL_VALUE);

    // Exclusão mútua para secção crítica.
    sem_t* mutex = sem_open(MUTEX_GENERIC_NAME, O_CREAT, 0644, MUTEX_INITAL_VALUE);

    // Recursos limitados (club).
    sem_t* lim_res = sem_open(LIMITED_RES_GENERIC_NAME, O_CREAT, 0644, CLUB_CAPACITY);

    if(club == SEM_FAILED || mutex == SEM_FAILED || lim_res == SEM_FAILED){
        perror("sem_open");
        exit(EXIT_FAILURE);
    }

    int fd, size = sizeof(info_t);
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

    // Atribuição do espaço livre do club na memória partilhada.
    addr->free_space = CLUB_CAPACITY;

    // Inicialização dos clientes criados (efeitos de sincronização de processos para uma execução menos enviesada).
    addr->created_clients = 0;

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

    // Implementação de um algoritmo de barreira.
    sem_wait(mutex);
    if(addr->created_clients == CLIENTS){
        sem_post(barrier);
    }
    addr->created_clients += 1;
    sem_post(mutex);

    sem_wait(barrier);
    sem_post(barrier);

    printf("\n\n");

    // Processo club.
    if(i == CLIENTS){
        int iteration = 0;
        // Desincrementa o valor para que o último processo deixa para o processo fantasma.
        sem_wait(barrier);
        do{

            sem_wait(barrier);

            // O club vai abrindo as portas para as pessoas das mais prioritárias para as menos.
            for(int i=0;i<CLIENT_TYPES;i++){
                char txt[10];
                get_client_type(txt, NULL, i);
                printf("Opening the entrance to %s clients enter the club...\n\n\n", txt);
                sem_post(client_type[i]);
            }

            // Após x segundos, o cliente é sai do bar.
            int r = rand() % 5;
            sleep(r);

            // Indica que o cliente já saiu.
            sem_post(club);
            
        }while(++iteration < CLIENTS);
    }

    // Processos clientes.
    if(i < CLIENTS){

        // Caso haja espaço, vai ser possível desincrementar. Caso contrário, aguarda.
        sem_wait(lim_res);

        // Acorda o club para que possa receber o cliente.
        sem_post(barrier);

        char txt[10];

        int type = get_client_type(txt, array, i);

        printf("Client %d [%s] is awaiting in the line of the club...\n", getpid(), txt);

        // Acede exclusivamente à memória partilhada para atualizar o número de clientes no bar.
        sem_wait(mutex);
        printf("Free space in the club: %d\n", addr->free_space);
        addr->free_space -= 1;
        sem_post(mutex);

        // Espera pela permissão do "porteiro" para entrar no club.
        sem_wait(client_type[type]);

        printf("Client %d entered the club...\n", getpid());

        // Está no club...
        sem_wait(club);

        // Vai sair do club e vai atualizar na memória partilhada.
        sem_wait(mutex);
        printf("Free space in the club: %d\n", addr->free_space);
        addr->free_space += 1;
        sem_post(lim_res);

        printf("%d client left the club...\n\n", getpid());
        sem_post(mutex);
        
        exit(EXIT_SUCCESS);
    }
    
    for(int i=0;i<CLIENTS;i++){
        wait(NULL);
    }

    unlink_semaphores(CLIENT_SEM_NAME, CLIENT_TYPES);

    if(sem_unlink(CLUB_SEM_NAME) < 0 || sem_unlink(MUTEX_GENERIC_NAME) < 0 || sem_unlink(LIMITED_RES_GENERIC_NAME)){
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