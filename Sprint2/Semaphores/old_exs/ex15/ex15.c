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


#define LOBBIES 1
#define ROOMS 4
#define VISITORS 25
#define SHOW_DURATION 5
#define ROOM_SPACE 5
#define MUTEX_NAME "/mutex"
#define BARRIER_NAME "/barrier"
#define LIMITED_RESOURCES_NAME "/limited_resources"
#define SHARED_MEMORY_NAME "/shared_memory"

typedef struct{
    int user_counter;
    int rooms[ROOMS][ROOM_SPACE];
}room_t;

/**
 * 
 *  
*/

void room_show(){
    printf("Show starting now...\n");
    sleep(SHOW_DURATION);
    printf("Show ending now...\n");
}

int main(){

    sem_t* mutex = sem_open(MUTEX_NAME, O_CREAT, S_IRUSR|S_IWUSR, MUTEX_INITAL_VALUE);
    sem_t* barrier = sem_open(MUTEX_NAME, O_CREAT, S_IRUSR|S_IWUSR, SYNCHRONIZATION_INITAL_VALUE);
    sem_t* limited_resources = sem_open(LIMITED_RESOURCES_NAME, O_CREAT, S_IRUSR|S_IWUSR, ROOM_SPACE);

    int fd, size = sizeof(int);
    int *addr;
    int r;

    fd = shm_open(SHARED_MEMORY_NAME, O_CREAT|O_RDWR, S_IRUSR|S_IWUSR);
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

    *(addr) = 0;

    pid_t p;
    for(int i=0;i<VISITORS;i++){
        p = fork();
        if(p < 0){
            perror("fork");
            exit(EXIT_FAILURE);
        }
    }

    sem_wait(mutex);
    *(addr) += 1;
    if(*(addr) == VISITORS){
        sem_post(barrier);
    }
    sem_post(mutex);

    sem_wait(barrier);
    sem_post(barrier);

    if(p == 0){
        for(;;){

        }
    }



    exit(EXIT_SUCCESS);
}