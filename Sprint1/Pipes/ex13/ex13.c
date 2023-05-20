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

#define MACHINES 4
#define PIECES 1000
#define FLUX_OF_PIECES 4
#define READ 0
#define WRITE 1

int main(){
    
    int single_pipes[FLUX_OF_PIECES][2];

    pid_t p[MACHINES];
    
    for(int i=0;i<FLUX_OF_PIECES;i++){
        if(pipe(single_pipes[i]) < 0){
        perror("Pipe");
        exit(EXIT_FAILURE);
        }
    }

    for(int i=0;i<MACHINES;i++){
        p[i] = fork();

        if(p[i] < 0){
        perror("Fork");
        exit(EXIT_FAILURE);
        }
        
        // Código das máquinas.
        if(p[i] == 0){
            
            // Código da M1.
            if(i == 0){
                
                for(int i=0;i<MACHINES;i++){
                    close(single_pipes[i][READ]);
                    if(i != 0)
                        close(single_pipes[i][WRITE]);
                }
                

                
                int pieces = PIECES;
                while(pieces > 0){
                    pieces -= 5;
                    
                    // Cutting pieces...
                    int cut_pieces = 5;
                    
                    write(single_pipes[i][WRITE], &cut_pieces, sizeof(int));
                }
                close(single_pipes[i][WRITE]);
            }

            // Código da M2.
            if(i == 1){
                
                for(int i=0;i<MACHINES;i++){
                    if(i != 0)
                        close(single_pipes[i][READ]);
                    if(i != 1)
                        close(single_pipes[i][WRITE]);
                }
                
                int cut_pieces;
                ssize_t bytes = 1;
                
                while(bytes > 0){
                    bytes = read(single_pipes[i-1][READ], &cut_pieces, sizeof(int));
                    
                    // Se entrar nesta condição, é porque o read foi desbloqueado.
                    // Ou seja, todos os processos que podiam escrever para aqui, fecharam esta "porta".
                    if(bytes == 0){
                        continue;
                    }
                        
                    // Folding the cut pieces...
                    int folded_pieces = cut_pieces;
                    
                    write(single_pipes[i][WRITE], &folded_pieces, sizeof(int));
                }
                close(single_pipes[i-1][READ]);
                close(single_pipes[i][WRITE]);
            }

            // Código da M3.
            if(i == 2){
                
                for(int i=0;i<MACHINES;i++){
                    if(i != 1)
                        close(single_pipes[i][READ]);
                    if(i != 2)
                        close(single_pipes[i][WRITE]);
                }
                
                int folded_pieces = 0;
                int received_folded_pieces = 0;
                ssize_t bytes = 1;
                while(bytes > 0){
                    while(folded_pieces < 10){
                        bytes = read(single_pipes[i-1][READ], &received_folded_pieces, sizeof(int));
                        if(bytes == 0)
                            break;
                        folded_pieces += received_folded_pieces;
                    }
                    if(bytes == 0){
                        continue;
                    }
                    printf("> Already received %d folded pieces, the process is starting...\n", folded_pieces);
                    
                    // Welding the folded pieces...
                    int welded_pieces = folded_pieces;
                    
                    write(single_pipes[i][WRITE], &welded_pieces, sizeof(int));
                    folded_pieces = 0;
                }
                close(single_pipes[i-1][READ]);
                close(single_pipes[i][WRITE]);
            }

            // Código da M4.
            if(i == 3){
                for(int i=0;i<MACHINES;i++){
                    if(i != 2)
                        close(single_pipes[i][READ]);
                    if(i != 3)
                        close(single_pipes[i][WRITE]);
                }
                ssize_t bytes = 1;
                int welded_pieces = 0;
                int received_welded_pieces = 0;
                while(bytes > 0){
                    while(welded_pieces < 100){
                        bytes = read(single_pipes[i-1][READ], &received_welded_pieces, sizeof(int));
                        welded_pieces += received_welded_pieces;
                    }
                    if(bytes == 0){
                        continue;
                    }
                    printf("> Already received %d welded pieces, the process is starting...\n", welded_pieces);
                    
                    // Packing the welded pieces...
                    int packed_pieces = welded_pieces;
                    
                    write(single_pipes[i][WRITE], &packed_pieces, sizeof(int));
                    welded_pieces = 0;
                }
                close(single_pipes[i-1][READ]);
                close(single_pipes[i][WRITE]);
            }
        
            exit(EXIT_SUCCESS);
        }
    }

    // Código do armazém.
    for(int i=0;i<MACHINES;i++){
        if(i != MACHINES-1)
            close(single_pipes[i][READ]);
            
        close(single_pipes[i][WRITE]);
    }
    
    // Enquanto o armazém não receber 1000 peças, vai continuar à espera da máquina M4 para receber peças empacotadas.
    int packed_pieces = 0;
    while(packed_pieces < PIECES){
        int received_packed_pieces;
        read(single_pipes[FLUX_OF_PIECES-1][READ], &received_packed_pieces, sizeof(int));
        packed_pieces += received_packed_pieces;
    }
    
    close(single_pipes[FLUX_OF_PIECES-1][READ]);
    
    printf("\n\n> %d packed pieces were received!\n\n> Turning all machines off...", packed_pieces);

    exit(EXIT_SUCCESS);
}