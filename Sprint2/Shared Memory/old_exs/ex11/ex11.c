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

#define CHILDREN 10
#define PIPES CHILDREN
#define ARRAY_SIZE 1000
#define MAX_VALUE_TO_GENERATE 1000
#define READ 0
#define WRITE 1

void fill_array(int* array, int size){
    srand(time(NULL));
    for(int i=0;i<size;i++){
        *(array+i) = rand() % MAX_VALUE_TO_GENERATE;
    }
}

int search_maximum_value(int* array, int min_lim, int max_lim, int max_value){
    if(min_lim == max_lim)
        return max_value;
    
    if(*(array+min_lim) > max_value)
        max_value = *(array+min_lim);
    
    return search_maximum_value(array, min_lim+1, max_lim, max_value);
    
}

int main(){

    int* array = (int*)calloc(ARRAY_SIZE, sizeof(int));

    fill_array(array, ARRAY_SIZE);
    
    int fd[2];
    
    if(pipe(fd) < 0){
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
            
            close(fd[READ]);
            
            int min = i * ARRAY_SIZE/CHILDREN, max = (i+1) * ARRAY_SIZE/CHILDREN;

            int partial_maximum_value = search_maximum_value(array, min, max, 0);

            write(fd[WRITE], &partial_maximum_value, sizeof(int));
            
            close(fd[WRITE]);
            
            exit(EXIT_SUCCESS);
        }
    }
    
    close(fd[WRITE]);
    
    int maximum_value = 0, aux;
    for(int i=0;i<CHILDREN;i++){
        read(fd[READ], &aux, sizeof(int));
        if(aux > maximum_value){
            maximum_value = aux;
        }
    }
    
    close(fd[READ]);
    
    printf("\n\tGreatest number generated: %d\n", maximum_value);
    
    exit(EXIT_SUCCESS);
}