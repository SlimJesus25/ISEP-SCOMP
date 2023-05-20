#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/syscall.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <sys/wait.h>
#include <ctype.h>

#define CHILDREN 5
#define PIPES CHILDREN + 1
#define MAX_VALUE_TO_GENERATE 500
#define MIN_VALUE_TO_GENERATE 1
#define READ 0
#define WRITE 1

int main(){
    
    // Matriz de pipes.
    int fd[PIPES][2];
    
    for(int i=0;i<PIPES;i++){
        if(pipe(fd[i]) < 0){
        perror("Pipe");
        exit(EXIT_FAILURE);
        }
    }

    for(int i=0;i<CHILDREN;i++){
        pid_t p = fork();

        if(p < 0){
        perror("Fork");
        exit(EXIT_FAILURE);
        }
        
        // Código dos filhos.
        if(p == 0){
            
            close(fd[i][WRITE]);

            // Cada filho cria a sua seed (com base no seu pid para evitar valores iguais).
            srand(getpid());
            
            int rand_value = rand() % (MAX_VALUE_TO_GENERATE - MIN_VALUE_TO_GENERATE + 1) + MIN_VALUE_TO_GENERATE,
            received_value;
            
            printf("[PID:%d]Number Generated: %d\n", getpid(), rand_value);
            
            // Armazena o valor recebido pelo pipe na variável received_value.
            read(fd[i][READ], &received_value, sizeof(int));
            
            close(fd[i][READ]);
            
            if(received_value > rand_value){
                rand_value = received_value;
            }
            
            int next = i+1;
            close(fd[next][READ]);
            
            // Escreve para o próximo pipe o maior valor entre o recebido e o gerado.
            write(fd[next][WRITE], &rand_value, sizeof(int));
            
            close(fd[next][WRITE]);
            
            exit(EXIT_SUCCESS);
        }
    }
    
    srand(getpid());
    
    close(fd[0][READ]);
    int rand_value = rand() % (MAX_VALUE_TO_GENERATE - MIN_VALUE_TO_GENERATE + 1) + MIN_VALUE_TO_GENERATE;
    printf("[PID:%d]Number Generated: %d\n", getpid(), rand_value);
    write(fd[0][WRITE], &rand_value, sizeof(int));
    
    close(fd[0][WRITE]);
    close(fd[CHILDREN][WRITE]);
    
    read(fd[CHILDREN][READ], &rand_value, sizeof(int));
    
    close(fd[CHILDREN][READ]);
    
    printf("\n\tGreatest number generated: %d", rand_value);
    

    exit(EXIT_SUCCESS);
}