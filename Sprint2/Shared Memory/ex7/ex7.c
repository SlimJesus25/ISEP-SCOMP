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
Implement a solution that allows you to share an array of 10 integers between two processes not related
hierarchically, a writer and a reader.
• The writer must create a shared memory area, generate 10 random numbers between 1 and 20 and write
them in the shared memory.
• The reader should read the 10 values, calculate and print the average.
Note: the writer must be executed before the reader.
*/

#define SIZE 10
#define MIN 1
#define MAX 20

typedef struct{
    int values[SIZE];
    int stop;
}gen_i;

int main(){

    srand((unsigned) time(NULL));

    int fd, data_size = sizeof(gen_i);

    gen_i *ptr;

    fd = shm_open("/ex7shrdmem", O_CREAT|O_EXCL|O_RDWR, S_IRUSR|S_IWUSR);
    ftruncate(fd, data_size);
    ptr = (gen_i*)mmap(NULL, data_size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
    ptr->stop = 0;
    pid_t p = fork();

    if(p < 0){
        perror("Error creating process!");
        return EXIT_FAILURE;
    }
    if(p == 0){ // Writer
       for(int i=0;i<SIZE;i++)
            ptr->values[i] = MIN + rand() % (MAX - MIN);
       ptr->stop = 1;
       shm_unlink("/ex7shrdmem");
       exit(EXIT_SUCCESS);
    }else if(p > 0){ // Reader
        float sum=0;
        while(ptr->stop == 0);
        for(int i=0;i<SIZE;i++){
            sum+=ptr->values[i];
            //printf("%d\n", ptr->values[i]);
        }

        printf("Average of generated values: %.2f\n", sum/SIZE);

        exit(EXIT_SUCCESS);
    }

    shm_unlink("/ex7shrdmem");

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