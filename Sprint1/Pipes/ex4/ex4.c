#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/syscall.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <sys/wait.h>

#define MAX_SIZE 50000

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
        printf("\tReceived message\n\n\"%s\"", msg);
        close(fd[0]);
        exit(EXIT_SUCCESS);
    }
    if(p > 0){
        close(fd[0]);
        char str_value[MAX_SIZE];

        FILE *ptr;

        ptr = fopen("file.txt", "r");
        char line;
        
        if(ptr != NULL){
            fgets(str_value, MAX_SIZE, ptr);
            write(fd[1], str_value, strlen(str_value));
        }

        fclose(ptr);

        close(fd[1]);
    }
    
    int status;

    p = wait(&status);
    printf("\n\n\tChild status\nPID: %d\nExit value: %d", p, WEXITSTATUS(status));

    exit(EXIT_SUCCESS);
}