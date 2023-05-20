#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/syscall.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <sys/wait.h>

#define SIZE 1000000

void fill_int_array(int* addr){
    for(int i=0;i<SIZE;i++){
        addr[i] = i;
    }
}

int main(){
    
    int fd[2];
    if(pipe(fd) < 0){
        perror("Pipe");
        exit(EXIT_FAILURE);
    }
    
    pid_t p = fork();
    if(p < 0){
        perror("Fork");
        exit(EXIT_FAILURE);
    }
    if(p == 0){
        close(fd[1]);
        int* local_array = (int*)calloc(SIZE, sizeof(int));

        clock_t ini = clock();
        printf("\n[READER] Reading from pipe...\n");
        for(int i=0;i<SIZE;i++){
            read(fd[0], &local_array[i], sizeof(int));
        }
        clock_t end = clock();
        double time = (double)(end-ini)/CLOCKS_PER_SEC;
        printf("\n[READER] Operation took %f seconds\n", time);

        printf("[READER] First content received: %d\n[READER] Middle content received: %d\n[READER] Last content received: %d",
         local_array[0], local_array[SIZE/2], local_array[SIZE-1]);

        close(fd[0]);
        exit(EXIT_SUCCESS);
    }
    if(p > 0){
        close(fd[0]);
        int local_array[SIZE];

        fill_int_array(local_array);

        printf("\n[WRITER] Writing on pipe...\n");
        clock_t ini = clock();
        for(int i=0;i<SIZE;i++){
            write(fd[1], &local_array[i], sizeof(int));
        }
        clock_t end = clock();
        double time = (double)(end-ini)/CLOCKS_PER_SEC;
        printf("\n[WRITER] Write operation done! Operation took %f seconds\n", time);

        close(fd[1]);
    }
    
    wait(NULL);

    exit(EXIT_SUCCESS);
}