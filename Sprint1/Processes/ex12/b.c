#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <time.h>

#define CHILD_NUMBER 6

int spawn_childs(int n){
    pid_t p;
    for(int i=0;i<n;i++){
        p = fork();
        if(p == 0){
            return i+1;
        }
    }
    return 0;
}

int main(){

    int index = spawn_childs(CHILD_NUMBER);
    pid_t pid;

    if(index != 0){
        printf("[%d] quiting with value %d\n", getpid(), index*2);
        exit(index * 2);
    }

    for(int i=0;i<CHILD_NUMBER;i++){
        int status;
        pid = wait(&status);
        int exit_value = WEXITSTATUS(status);
        printf("[Child %d died] %dº exiting: %d\n", pid, i+1, exit_value);
    }

    return 0;
}