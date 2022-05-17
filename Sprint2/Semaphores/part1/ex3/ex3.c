#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <signal.h>
#include <sys/time.h>
#include <semaphore.h>
#include <math.h>

typedef struct{
char str[80];
int actualSize;
}test_t;

int main(){

    test_t *ptr;

    int fd, data_size = 50*sizeof(ptr);

    srand((unsigned) time(NULL));

    int wait_time = rand()%5;

    fd = shm_open("/ex3shmem1", O_CREAT|O_RDWR, S_IRUSR|S_IWUSR);
    ftruncate(fd, data_size);
    ptr = (test_t*)mmap(NULL, data_size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);

    sem_t *sem; // Semaphore pointer.

    // Creates semaphore and tests it is valid.
    if((sem = sem_open("exsem3new1", O_CREAT, 0644, 1)) == SEM_FAILED){
        perror("Error creating semaphore!");
        exit(EXIT_FAILURE);
    }

    pid_t p;

    int i;
    for(i=0;i<50;i++){
        p = fork();
        if(p == 0)
            break;
    }

    if(p == 0){

        sem_wait(sem);
        char *pid;

        pid = (char*)calloc(80, sizeof(char));

        snprintf(pid, 80, "I'm the Father - with PID %d | %d", getpid(), i);
        if(ptr->actualSize == 50*80)
            perror("Shared memory is full!\n");
        else{

            strcpy((ptr+i)->str, pid);
            ptr->actualSize += 80;
            sleep(wait_time);
        }
        sem_post(sem);
        exit(EXIT_SUCCESS);
    }

    for(int i=0;i<50;i++)
        wait(NULL);

    for(i=0;i<50;i++)
        printf("%s\n", (ptr+i)->str);

    sem_close(sem);
    sem_unlink("exsem3new1");

    shm_unlink("/ex3shmem1");

        if(munmap(ptr, data_size)){
            perror("Error disconnecting the shared memory area!\n");
            exit(EXIT_FAILURE);
        }

        if(close(fd) == EXIT_FAILURE){
            perror("Error closing file descriptor!\n");
            exit(EXIT_FAILURE);
        }

    return 0;
}