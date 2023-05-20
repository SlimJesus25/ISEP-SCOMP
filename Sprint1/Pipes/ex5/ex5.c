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

#define MAX_SIZE 256
#define ACKNOWLEDGE "ACK"

int main(){
    
    int up[2], down[2];
    
    if(pipe(up) < 0){
        perror("Pipe1");
        exit(EXIT_FAILURE);
    }

    if(pipe(down) < 0){
        perror("Pipe2");
        exit(EXIT_FAILURE);
    }
    
    pid_t p = fork();
    if(p < 0){
        perror("Fork");
        exit(EXIT_FAILURE);
    }
    
    // Processo "cliente".
    if(p == 0){
        char message_to_send[MAX_SIZE];
        char received_message[MAX_SIZE];

        close(up[0]);
        close(down[1]);

        printf("Mensagem a enviar ao servidor: ");
        scanf("%s", message_to_send);
        
        write(up[1], message_to_send, MAX_SIZE);

        read(down[0], received_message, MAX_SIZE);

        printf("Mensagem recebida do servidor: %s", received_message);

        exit(EXIT_SUCCESS);
    }

    // Processo "Servidor".
    if(p > 0){
        char message_to_send[MAX_SIZE];
        char received_message[MAX_SIZE];

        close(up[1]);
        close(down[0]);

        read(up[0], received_message, MAX_SIZE);

        int i=0;
        while(received_message[i] != '\0'){
            if(isupper(received_message[i]) > 0){
                received_message[i++] = tolower(received_message[i]);
            }else if(islower(received_message[i]) > 0){
                received_message[i++] = toupper(received_message[i]);
            }else{
                i++;
            }
        }

        write(down[1], received_message, MAX_SIZE);

    }
    
    int status;

    p = wait(&status);
    printf("\n\n\tChild status\nPID: %d\nExit value: %d", p, WEXITSTATUS(status));

    exit(EXIT_SUCCESS);
}