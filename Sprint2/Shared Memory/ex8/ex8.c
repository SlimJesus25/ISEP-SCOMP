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
#include <time.h>

#define OPERATIONS 1000000

typedef struct{
    int n;
}struct1;

int main(){
    int fd;
    int data_size=sizeof(struct1);

    struct1 *ptr;

    fd = shm_open("/shrdmmry8", O_CREAT|O_EXCL|O_RDWR, S_IRUSR|S_IWUSR);
    ftruncate(fd, data_size);

    ptr = mmap(NULL, data_size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);

    ptr->n=100;

    pid_t p = fork();

    if(p < 0){
        perror("Error creating process!");
        return EXIT_FAILURE;
    }

    for(int i=0;i<OPERATIONS;i++){
        ptr->n++;
        ptr->n--;
    }

    printf("Value : %d\n",ptr->n);

    close(fd);

    if(p==0){
        exit(1);
    }
    
    wait(NULL);
    shm_unlink("/shrdmmry8");

    return 0;
}