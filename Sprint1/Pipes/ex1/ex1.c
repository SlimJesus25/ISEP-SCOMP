#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/syscall.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <sys/wait.h>

#define MAX_SIZE 20

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
        char msg[MAX_SIZE];
        read(fd[0], msg, MAX_SIZE);
        printf("[CHILD] Message received: %s", msg);
        close(fd[0]);
        exit(EXIT_SUCCESS);
    }
    if(p > 0){
        close(fd[0]);
        char msg[MAX_SIZE];
        snprintf(msg, MAX_SIZE, "My PID %d", getpid());
        write(fd[1], msg, MAX_SIZE);
        close(fd[1]);
    }
    
    wait(NULL);

    exit(EXIT_SUCCESS);
}