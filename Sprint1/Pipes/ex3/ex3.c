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
        char fst_msg[MAX_SIZE];
        char scd_msg[MAX_SIZE];
        
        read(fd[0], fst_msg, MAX_SIZE);
        printf("First message: %s", fst_msg);

        read(fd[0], scd_msg, MAX_SIZE);
        printf("\nSecond message: %s", scd_msg);
        close(fd[0]);
        exit(EXIT_SUCCESS);
    }
    if(p > 0){
        close(fd[0]);
        char fst_msg[MAX_SIZE] = "Hello World!";
        char scd_msg[MAX_SIZE] = "Goodbye!";

        write(fd[1], fst_msg, MAX_SIZE); 
        write(fd[1], scd_msg, MAX_SIZE);
        close(fd[1]);
    }
    
    int status;
    p = wait(&status);
    printf("\n\n\tChild status\nPID: %d\nExit value: %d", p, WEXITSTATUS(status));

    exit(EXIT_SUCCESS);
}