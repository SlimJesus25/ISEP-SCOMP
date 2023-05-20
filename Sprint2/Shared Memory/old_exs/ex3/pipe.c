#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/syscall.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <sys/wait.h>

#define CONST_STR "ISEP - SCOMP 2023"
#define SIZE 100000

typedef struct{
    int num;
    char str[18];
}test_t;

void fill_test_t_array(test_t* addr){
    for(int i=0;i<SIZE;i++){
        addr[i].num = i;
        strcpy(addr[i].str, CONST_STR);
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
        test_t* local_array = (test_t*)calloc(SIZE, sizeof(test_t));

        clock_t ini = clock();
        printf("\n[READER] Reading from pipe...\n");

        for(int i=0;i<SIZE;i++){
            read(fd[0], &local_array[i], sizeof(test_t));
        }

        clock_t end = clock();
        double time = (double)(end-ini)/CLOCKS_PER_SEC;
        printf("\n[READER] Operation took %f seconds\n", time);

        printf("[READER] First content received: %d|%s \n[READER] Middle content received: %d|%s \n[READER] Last content received: %d|%s",
         local_array->num, local_array->str,(local_array+SIZE/2)->num, (local_array+SIZE/2)->str, (local_array+SIZE-1)->num, (local_array+SIZE-1)->str);

        close(fd[0]);
        exit(EXIT_SUCCESS);
    }
    if(p > 0){
        close(fd[0]);
        test_t local_array[SIZE];

        fill_test_t_array(local_array);

        printf("\n[WRITER] Writing on pipe...\n");
        clock_t ini = clock();

        for(int i=0;i<SIZE;i++){
            write(fd[1], &local_array[i], sizeof(test_t));
        }

        clock_t end = clock();
        double time = (double)(end-ini)/CLOCKS_PER_SEC;
        printf("\n[WRITER] Write operation done! Operation took %f seconds\n", time);

        close(fd[1]);
    }
    
    wait(NULL);

    exit(EXIT_SUCCESS);
}