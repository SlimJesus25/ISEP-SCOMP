#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

#define CHILDREN 2

int main(){

    int status;

    for(int i=0;i<CHILDREN;i++){
        pid_t p = fork();
        if(p == 0){
            sleep(i+1);
            exit(i+1);
        }
    }

    for(int i=0;i<CHILDREN;i++){
        int pid = wait(&status);
        printf("Valor de saída do processo PID %d: %d\n", pid, WEXITSTATUS(status));
    }

    exit(EXIT_SUCCESS);
}