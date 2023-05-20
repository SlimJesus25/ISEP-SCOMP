#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <time.h>

#define CHILD_NUMBER 10
#define ARRAY_SIZE 2000

int first_occurrence_of_number(int* array, int number_to_search, int min_limit, int max_limit){
    for(int i=min_limit; i<max_limit;i++){
        if(array[i] == number_to_search){
            return i;
        }
    }
    return -1;
}

void print_integer_array(int* array){
    for(int i=0;i<ARRAY_SIZE;i++){
        if(i % 100 == 0){
            printf("\n");
        }
        printf("%d,", array[i]);
    }
}

void generate_random_integer_array(int* array){
    time_t t;
    srand ((unsigned) time (&t));

    for (int i = 0; i < ARRAY_SIZE; i++)
        array[i] = rand () % 1000;
}

int main(){

    int* array = (int*) calloc(ARRAY_SIZE, sizeof(int));
    
    generate_random_integer_array(array);
    
    //print_integer_array(array);
    printf("\n\n");

    int n = rand () % 1000;

    printf("Value of n: %d\n", n);

    pid_t p;
    int min = 0;
    int incremental = ARRAY_SIZE/CHILD_NUMBER;
    int max = incremental;

    for(int i=0;i<CHILD_NUMBER;i++){
        p = fork();
        if(p == 0){
            int index = first_occurrence_of_number(array, n, min, max);
            if(index < 0){
                exit(255);
            }else{
                exit(index);
            }
        }
        min = max;
        max += incremental;
    }
    
    printf("\n\n");
    int status;
    for(int i=0;i<CHILD_NUMBER;i++){
        pid_t pid = wait(&status);
        int index = WEXITSTATUS(status);
        if(index < 255){
            printf(" [PID: %d] Found %d in a valid index: %d\n", pid, n, index);
        }
    }

    return 0;
}