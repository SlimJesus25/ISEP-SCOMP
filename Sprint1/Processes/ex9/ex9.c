#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <time.h>

#define CHILD_NUMBER 10

void print_number_set(int min_limit, int max_limit){
    for(int i=min_limit; i<=max_limit;i++){
        printf("%d,", i);
    }
}

int main(){

    printf("\n\n");

    pid_t p;
    int min=0;
    int max=0;

    for(int i=0;i<CHILD_NUMBER;i++){
        min = max + 1;
        max = min + 99;
        p = fork();
        if(p == 0){
            print_number_set(min, max);
            printf("\n\n");
            exit(EXIT_SUCCESS);
        }
    }

    for(int i=0;i<CHILD_NUMBER;i++){
        wait(NULL);
    }

    return 0;
}