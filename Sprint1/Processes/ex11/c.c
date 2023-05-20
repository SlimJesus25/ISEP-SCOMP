#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <time.h>

#define CHILD_NUMBER 10
#define ARRAY_SIZE 1000
#define RANGE_MINIMUM 0
#define RANGE_MAXIMUM 255

void generate_random_integer_array(int* array, int range_min, int range_max){
    time_t t;
    srand ((unsigned) time (&t));

    for (int i = 0; i < ARRAY_SIZE; i++)
        array[i] = rand () % (range_max - range_min) + range_min;
}

int maximum_value(int* array, int min, int max){
    int max_value = array[min];
    for(int i=min+1;i<max;i++){
        if(array[i] > max_value){
            max_value = array[i];
        }
    }
    return max_value;
}

void calculations(int* result, int* numbers, int max_value, int min_range, int max_range){
    for(int i=min_range;i<max_range;i++){
        result[i] = ((int) numbers[i]/max_value)*100;
    }
}

int main(){

    int* array = (int*) calloc(ARRAY_SIZE, sizeof(int));
    
    generate_random_integer_array(array, RANGE_MINIMUM, RANGE_MAXIMUM);

    pid_t p;

    int min = 0;
    int incremental = ARRAY_SIZE/CHILD_NUMBER;
    int max = incremental;

    for(int i=0;i<CHILD_NUMBER;i++){
        p = fork();
        if(p == 0){
            int max_value = maximum_value(array, min, max);
            exit(max_value);
        }
        min = max;
        max += incremental;
    }

    int max_value_array = -1;
    for(int i=0;i<CHILD_NUMBER;i++){
        int status;
        wait(&status);
        int exit_value = WEXITSTATUS(status);
        if(max_value_array < exit_value){
            max_value_array = exit_value;
        }
    }

    int* result = (int*) calloc(ARRAY_SIZE, sizeof(int));

    p = fork();
    if(p == 0){
        calculations(result, array, max_value_array, 0, ARRAY_SIZE/2);
        
        exit(EXIT_SUCCESS);
    }
    calculations(result, array, max_value_array, ARRAY_SIZE/2, ARRAY_SIZE);
    wait(NULL);

    return 0;
}