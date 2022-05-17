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


typedef struct{
    char array[101];
    int active_wait;
}struct1;

int main(){
    int lower =65 , upper=90;    // A ... Z
    int fd, data_size = sizeof(struct1);

    struct1 *ptr;

    fd = shm_open("/shrdmmry", O_CREAT|O_EXCL|O_RDWR, S_IRUSR|S_IWUSR);
    ftruncate(fd, data_size);

    ptr = (struct1 *)mmap(NULL, data_size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);

    ptr->active_wait = 0;

    pid_t p = fork();

    if(p < 0){
        perror("Error creating process!");
        return EXIT_FAILURE;
    }

    if(p > 0){
        for(int i=0;i<100;i++){
            ptr->array[i] = (rand() % (upper-lower+1)) + lower;
        }

        ptr->active_wait = 1;

    }else{
        while(!ptr->active_wait);
        printf("Data in shared memory: %s\n", ptr->array);
        exit(0);
    }

    wait(NULL);
    shm_unlink("/shrdmmry");
    return 0;
}