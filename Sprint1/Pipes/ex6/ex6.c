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

#define MAX_SIZE 10
#define CHILDREN 5

void sum_arrays(int* array_sum, int* array_one, int* array_two, int min_lim, int max_lim){

    // The tmp array have 1/CHILDREN size because there's no need to send more data that what we are working here.
    int k=0;
    for(int i=min_lim;i<max_lim;i++){
        array_sum[k++] = array_one[i] + array_two[i];
    }
}

void assign_values_to_array(int* original, int* new_values, int min_lim, int max_lim){
    int k=0;
    for(int i=min_lim;i<max_lim;i++){
        original[i] = new_values[k++];
    }
}

void generate_random_integer_array(int* array, int max_size){

    for (int i = 0; i < max_size; i++)
        array[i] = rand () % 1000;
}

void print_array(int* array, int size){
    for(int i=0;i<size;i++){
        printf("%d|", array[i]);
    }
    printf("\n");
}

int main(){
    
    time_t t;
    srand ((unsigned) time (&t));

    int vec1[MAX_SIZE], vec2[MAX_SIZE], vec3[MAX_SIZE];

    // Generating random values to vec1 and vec2...
    generate_random_integer_array(vec1, MAX_SIZE);
    generate_random_integer_array(vec2, MAX_SIZE);
    printf("\n\tArray 1\n");
    print_array(vec1, MAX_SIZE);
    printf("\n\tArray 2\n");
    print_array(vec2, MAX_SIZE);

    int min=0, max=0;
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

        if(p == 0){
            close(fd[0]);

            min = i*(MAX_SIZE/CHILDREN);
            max = (i+1)*(MAX_SIZE/CHILDREN);

            int* tmp = calloc(MAX_SIZE/CHILDREN, sizeof(int));
            sum_arrays(tmp, vec1, vec2, min, max);
            write(fd[1], tmp, MAX_SIZE/CHILDREN*sizeof(int));

            close(fd[1]);
            exit(EXIT_SUCCESS);
        }
    }

    close(fd[1]);

    int* tmp = calloc(MAX_SIZE, sizeof(int));

    for(int i=0;i<CHILDREN;i++){
        min = i*(MAX_SIZE/CHILDREN);
        max = (i+1)*(MAX_SIZE/CHILDREN);

        read(fd[0], tmp, MAX_SIZE/CHILDREN*sizeof(int));
        assign_values_to_array(vec3, tmp, min, max);
        memset(tmp, 0, MAX_SIZE*sizeof(int));
    }
    close(fd[0]);

    printf("\n\tResult array\n");
    print_array(vec3, MAX_SIZE);




    exit(EXIT_SUCCESS);
}