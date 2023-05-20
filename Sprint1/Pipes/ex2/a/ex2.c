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
        int value;
        read(fd[0], &value, sizeof(int));
        read(fd[0], msg, MAX_SIZE);
        printf("\n\tValores recebidos do processo %d \nValor inteiro: %d\nValor alfanumérico: %s", getppid(), value, msg);
        close(fd[0]);
        exit(EXIT_SUCCESS);
    }
    if(p > 0){
        int int_value = 0;
        char str_value[MAX_SIZE];

        printf("Valor inteiro: ");
        scanf("%d", &int_value);
        printf("\nValor alfanumérico: ");
        scanf("%s", str_value);
        
        close(fd[0]);
        write(fd[1], &int_value, sizeof(int));
        write(fd[1], str_value, MAX_SIZE);
        close(fd[1]);
    }
    
    wait(NULL);

    exit(EXIT_SUCCESS);
}