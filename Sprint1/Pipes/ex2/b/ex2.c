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

typedef struct{
    char txt[MAX_SIZE];
    int val;
}pipmes_t;

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
        
        pipmes_t values;
        memset(&values, 0, sizeof(values));
        
        read(fd[0], &values, sizeof(values));
        printf("\n\tValores recebidos do processo %d \nValor inteiro: %d\nValor alfanumérico: %s", getppid(), values.val, values.txt);
        
        close(fd[0]);
        exit(EXIT_SUCCESS);
    }
    if(p > 0){
        pipmes_t values;
        memset(&values, 0, sizeof(values));

        printf("Valor inteiro: ");
        scanf("%d", &values.val);
        printf("\nValor alfanumérico: ");
        scanf(" %s", values.txt);
        
        close(fd[0]);
        write(fd[1], &values, sizeof(values));
        close(fd[1]);
    }
    
    wait(NULL);

    exit(EXIT_SUCCESS);
}