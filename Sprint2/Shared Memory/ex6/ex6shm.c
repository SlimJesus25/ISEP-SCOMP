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

#define ARRAY_SIZE 1000000
int array[ARRAY_SIZE];

typedef struct{
    int num;
    int active_wait_write;
    int active_wait_read;
}struct1;

void fill_array(){
    time_t t;
    srand((unsigned)time(&t));
    int i;
    for(i=0;i<ARRAY_SIZE;i++){
        array[i]=1+rand()%1000;
    }
}

int main(){
    int fd, data_size = sizeof(struct1);
    clock_t before = clock();
    int narray[ARRAY_SIZE];

    struct1 *ptr;

    fd = shm_open("/shrdmmry6", O_CREAT|O_EXCL|O_RDWR, S_IRUSR|S_IWUSR);
    ftruncate(fd, data_size);

    ptr = (struct1 *)mmap(NULL, data_size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);

    ptr->active_wait_read = 1;   // cant read
    ptr->active_wait_write = 0; // Can write

    pid_t p = fork();

    if(p < 0){
        perror("Error creating process!");
        return EXIT_FAILURE;
    }

    for(int i=0;i<ARRAY_SIZE;i++){

    if(p > 0){
        while(ptr->active_wait_write);
        ptr->num = array[i]; 
        ptr->active_wait_write = 1;
        ptr->active_wait_read = 0;

    }else{
        while(ptr->active_wait_read);
        narray[i] = ptr->num;
        ptr->active_wait_read = 1;
        ptr->active_wait_write = 0;
    }

    }

    if(p==0){
    clock_t difference = clock() - before;
    int timer=difference*1000/CLOCKS_PER_SEC;
    printf("SHM: %d seconds and %d miliseconds.\n",timer/1000,timer%1000);
    close(fd);
    exit(1);
    }

    close(fd);
    shm_unlink("/shrdmmry6");


        // PIPE

    int fdp[2];

    pipe(fdp);
    pid_t pid = fork();

    if(pid>0){

    close(fdp[0]);       //close read
    for(int j=0;j<ARRAY_SIZE;j++){
        write(fdp[1],&array[j],sizeof(int)); 
    }

    close(fdp[1]);

    } else {
    

    close(fdp[1]);

    for(int j=0;j<ARRAY_SIZE;j++){
        read(fdp[0],&narray[j],sizeof(int)); 
        }
    

    clock_t difference = clock() - before;
    int timer=difference*1000/CLOCKS_PER_SEC;
    printf("PIPE: %d seconds and %d miliseconds.\n",timer/1000,timer%1000);
    close(fdp[0]);
    exit(1);
    }

    return 0;
}