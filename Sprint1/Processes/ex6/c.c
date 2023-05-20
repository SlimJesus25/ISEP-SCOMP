#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

#define CHILDS 4

void main(){
    pid_t p;

    for(int i=0;i<CHILDS;i++){
        p = fork();
        if(p == 0){
            sleep(1);
            exit(EXIT_SUCCESS);
        }
    }


    for(int i=0;i<CHILDS;i++){
        p = wait(NULL);
        if(p % 2 != 0){
            printf("PID: %d\n", p);
        }
    }



    printf("This is the end.\n");
}