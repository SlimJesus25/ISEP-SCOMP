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

/**
 * Implement a program that creates a shared memory area to store two integers and initializes those integers with
the values 8000 and 200 and creates a new process. Parent and child must perform the following operations
1.000.000 times:
• The father will increase the first value and decrease the second value.
• The child will decrease the first value and increase the second value.
Only write the value on the screen at the end. Review the results. Will these results always be correct?
*/

#define SIZE 1000000
#define INT_AMOUNT 2


int main(){

    int *ptr;

    int fd, data_size = INT_AMOUNT*sizeof(int);

    fd = shm_open("/ex5shrdmem", O_CREAT|O_EXCL|O_RDWR, S_IRUSR|S_IWUSR);
    ftruncate(fd, data_size);
    ptr = (int*)mmap(NULL, data_size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);

    ptr[0] = 8000;
    ptr[1] = 200;

    pid_t p = fork();

    if(p < 0){
        perror("Error creating process!");
        return EXIT_FAILURE;
    }
    if(p > 0){
        for(int i=0;i<SIZE;i++){
            ptr[0]++;
            ptr[1]--;
        }
    }else if(p == 0){
        for(int i=0;i<SIZE;i++){
            ptr[0]--;
            ptr[1]++;
        }
        exit(EXIT_SUCCESS);
    }

    wait(NULL);

    printf("Final value\nFirst integer: %d\nSecond integer: %d\n", ptr[0], ptr[1]);

    shm_unlink("/ex5shrdmem");

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
