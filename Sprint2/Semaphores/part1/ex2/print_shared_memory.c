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
#include "info.h"

int main(){

    int fd, size = sizeof(info_t);
    info_t *addr;

    // Cria e abre uma zona de memória partilhada.
    fd = shm_open(SHARED_MEMORY_NAME, O_RDONLY, S_IRUSR);
    if(fd < 0){
        perror("shm_open");
        exit(EXIT_FAILURE);
    }

    // Mapeia a zona de memória partilhada no espaço do processo.
    addr = (info_t*)mmap(NULL, size, PROT_READ, MAP_SHARED, fd, 0);
    if(addr == MAP_FAILED){
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    for(int i=0;i<STRING_QUANTITY;i++){
        printf("%dº: %s\n", (i+1), addr->array_strings[i]);
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
    
    if(shm_unlink(SHARED_MEMORY_NAME) < 0){
        perror("shm_unlink");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}