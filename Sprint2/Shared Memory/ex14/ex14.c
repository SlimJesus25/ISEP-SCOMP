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

#define NUMEROS 10
#define LOOPS 3

typedef struct{
    int numeros[NUMEROS];
    int wait_write;
    int wait_read;
}struct1;

int main(){
    int fd;
    int data_size=sizeof(struct1);
    int localn=0;

    struct1 *ptr;

    fd = shm_open("/sh14x", O_CREAT|O_EXCL|O_RDWR, S_IRUSR|S_IWUSR);
    ftruncate(fd, data_size);

    ptr = mmap(NULL, data_size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);

    ptr->wait_write=0;
    ptr->wait_read=1;



    pid_t p = fork();

    if(p < 0){
        perror("Error creating process!");
        return EXIT_FAILURE;
    }

    for(int j=0;j<LOOPS;j++){
        for(int i=0;i<NUMEROS;i++){
            if(p==0){
            while(ptr->wait_read);
            printf("Consumer: %d\n",ptr->numeros[i]);
            ptr->wait_write=0;
            ptr->wait_read=1;
            } else {
                while(ptr->wait_write);
                localn=localn + ptr->numeros[i]+ 1;
                ptr->numeros[i]=localn;
                ptr->wait_read=0;
                ptr->wait_write=1;
            }
        }
    }

    if(p==0){
        exit(1);
    }
    
    wait(NULL);
    shm_unlink("/sh14x");

    return 0;
}