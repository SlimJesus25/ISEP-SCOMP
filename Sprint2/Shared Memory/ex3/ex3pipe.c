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
#include <sys/time.h>

#define SIZE 100000

typedef struct{
    int i;
    char str[18];
}test_t;

int main(){

    int fd[2];

    struct timeval begin_time, end_time;

    gettimeofday(&begin_time, NULL);

    if(pipe(fd) == -1){
        perror("Pipe failed!");
        return 1;
    }

    pid_t p = fork();

    if(p < 0){
        perror("Error creating process!");
        return EXIT_FAILURE;
    }
    if(p > 0){
        close(fd[0]);
        test_t arr[SIZE];
        for(int j=0;j<SIZE;j++){
            (arr)->i = j;
            strcpy((arr)->str, "ISEP - SCOMP 2020");
            write(fd[1], arr, sizeof(test_t));
        }

        printf("Array of struct test_t sent successfully!\n");
        close(fd[1]);
    }
    if(p == 0){
        close(fd[1]);
        test_t arr_chld[SIZE];

        printf("Array of test_t [Size %d] received successfully!\n", SIZE);
        for(int j=0;j<SIZE;j++){
            read(fd[0], arr_chld, sizeof(test_t));
            printf("%d | %s\n", (arr_chld)->i, (arr_chld)->str);
        }
        close(fd[0]);
        exit(EXIT_SUCCESS);
    }
    wait(NULL);
    gettimeofday(&end_time, NULL);
    printf("\nTook %ld seconds to complete through pipe\n", end_time.tv_sec - begin_time.tv_sec);

    return 0;
}