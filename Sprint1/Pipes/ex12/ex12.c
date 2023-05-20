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
#define PIPES CHILDREN
#define MAX_VALUE_TO_GENERATE 5
#define MIN_VALUE_TO_GENERATE 1
#define READ 0
#define WRITE 1

typedef struct{
    char description[64];
    int stock;
    int product_id;
}product_t;

typedef struct{
    int iteration_number;
    int product_id;
}info_t;

int main(){
    
    product_t array[5];
    
    for(int i=0;i<5;i++){
        strcpy(array[i].description, "description");
        array[i].stock = i*10+1;
        array[i].product_id = i+1;
    }
    
    // Pipe de comunicação singular entre pai e cada um dos "barcode readers".
    int single_pipes[PIPES][2];
    
    for(int i=0;i<PIPES;i++){
        if(pipe(single_pipes[i]) < 0){
        perror("Pipe");
        exit(EXIT_FAILURE);
        }
    }
    
    // Pipe de escrita de todos os "barcode readers".
    int global_pipe[2];
    
    if(pipe(global_pipe) < 0){
        perror("Pipe");
        exit(EXIT_FAILURE);
    }

    for(int i=0;i<CHILDREN;i++){
        pid_t p = fork();

        if(p < 0){
        perror("Fork");
        exit(EXIT_FAILURE);
        }
        
        // Código dos filhos.
        if(p == 0){
            
            close(global_pipe[READ]);
            close(single_pipes[i][WRITE]);

            // Cada filho cria a sua seed (com base no seu pid para evitar valores iguais).
            srand(getpid());
            
            while(1 == 1){
            
                // Gera tempo para dormir durante um bocado (para evitar spam).
                int sleep_time = rand() % 10;
                
                // Gera um product_id (sempre válido) para pedir informação ao processo pai.
                int product_id = rand() % (MAX_VALUE_TO_GENERATE - MIN_VALUE_TO_GENERATE + 1) + MIN_VALUE_TO_GENERATE;
                
                sleep(sleep_time);
                
                // Estrutura que vai identificar o pipe que o pai deve escrever + informação de que produto.
                info_t info;
                info.iteration_number = i;
                info.product_id = product_id;
                
                write(global_pipe[WRITE], &info, sizeof(info_t));
                
                product_t product_info;
                
                read(single_pipes[i][READ], &product_info, sizeof(product_t));
                
                printf("\tProduct %d Information\nStock: %d\nProduct ID: %d\nDescription: %s\n\n",
                product_id, product_info.stock, product_info.product_id, product_info.description);
            }
            
            exit(EXIT_SUCCESS);
        }
    }
    
    close(global_pipe[WRITE]);
    for(int i=0;i<PIPES;i++){
        close(single_pipes[i][READ]);
    }
    
    // Para não ficar um programa infinito, passados 15 segundos é enviado um SIGALRM
    //  para terminar o programa e já com resultados visíveis que o programa funciona.
    alarm(15);
    while(1 == 1){
        info_t info;
        read(global_pipe[READ], &info, sizeof(info_t));
        for(int i=0;i<5;i++){
            if(array[i].product_id == info.product_id){
                write(single_pipes[info.iteration_number][WRITE], array+i, sizeof(product_t));
            }
        }
    }

    exit(EXIT_SUCCESS);
}