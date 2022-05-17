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

/*
Implement a solution that tests the speed of two transfer methods between two processes – pipes and shared
memory.
  - Start by filling an array of 100 000 structures with an integer and the “ISEP – SCOMP 2020” string.
  - Start a timer and copy that array between the two processes using shared memory. Start a timer and use
a pipe to transfer the same amount of data between processes.
*/

#define SIZE 100000

volatile int stop;

typedef struct{
    int i;
    char str[18];
    int stop;
}test_t;

void handler_sigint(){
    shm_unlink("/ex3shrdmem2");
    write(STDOUT_FILENO, "Successfully exited shared memory!\n", 36);
    exit(EXIT_FAILURE);
}

int main(){

    struct timeval begin_time, end_time;

    struct sigaction act; // We are using signals due to halfway process interruptions.
                          // If some interruption is made before the code arrives to final, shared memory will be unlinked.

    memset(&act, 0, sizeof(act));

    act.sa_handler = handler_sigint;

    test_t *ptr;

    int fd, data_size = SIZE*sizeof(test_t);

    fd = shm_open("/ex3shrdmem2", O_CREAT|O_EXCL|O_RDWR, S_IRUSR|S_IWUSR);
    ftruncate(fd, data_size);
    ptr = (test_t*)mmap(NULL, data_size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);

    sigaction(SIGINT, &act, NULL);

    stop = 1;

    gettimeofday(&begin_time, NULL);
    pid_t p = fork();

    if(p < 0){
        perror("Error creating process!");
        return EXIT_FAILURE;
    }
    if(p > 0){
        for(int j=0;j<SIZE;j++){
            (ptr+j)->i = j;
            strcpy((ptr+j)->str, "ISEP – SCOMP 2020");
            (ptr+j)->stop = 1;
        }

        (ptr+SIZE-1)->stop=0;

    }else if(p == 0){
        while((ptr+SIZE-1)->stop != 0);
        for(int j=0;j<SIZE;j++)
            printf("%d | %s\n", (ptr+j)->i, (ptr+j)->str);
        exit(EXIT_SUCCESS);
    }

    wait(NULL);
    gettimeofday(&end_time, NULL);
    printf("\nTook %ld seconds to complete through shared memory\n", end_time.tv_sec - begin_time.tv_sec);

    if(munmap(ptr, data_size)){
        perror("Error disconnecting the shared memory area!\n");
        exit(EXIT_FAILURE);
    }

    if(close(fd) == EXIT_FAILURE){
        perror("Error closing file descriptor!\n");
        exit(EXIT_FAILURE);
    }

    kill(getpid(), SIGINT);

    return 0;

}