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
            exit(i+1);
        }
    }

    int status;
    for(int i=0;i<CHILDS;i++){
        p = wait(&status);
        if(p % 2 != 0){
            int exit_value = WEXITSTATUS(status);
            printf("PID: %d (EV: %d)\n\n", p, exit_value);
        }
    }

    printf("This is the end.\n");
}