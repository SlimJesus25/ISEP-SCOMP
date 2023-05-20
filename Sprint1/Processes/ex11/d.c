#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <time.h>

#define CHILD_NUMBER 10
#define ARRAY_SIZE 50
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

void calculations(float* result, int* numbers, int max_value, int min_range, int max_range){
    for(int i=min_range;i<max_range;i++){
        *(result+i) = ((float)numbers[i]/(float)max_value)*(float)100;
        if(i == min_range)
            printf("\n\nI'm %d and result[%d]: %.2f\n\n", getpid(), i, result[i]);
    }
}

void print_integer_array(float* array, int min_range, int max_range){
    char* buffer = (char*)calloc(2, sizeof(float));
    for(int i=min_range;i<max_range;i++){
        snprintf(buffer, sizeof(float)+3, "%.2f|", array[i]);
        write(STDOUT_FILENO, buffer, sizeof(float)+3);
        //printf("%.2f|", array[i]);
    }
}

int main(){
    printf("1\n");
    int* array = (int*) calloc(ARRAY_SIZE, sizeof(int));
    printf("2\n");
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

    float* result = (float*) calloc(ARRAY_SIZE, sizeof(float));
    p = fork();

    if(p == 0){
        min = 0;
        max = ARRAY_SIZE/2;
        calculations(result, array, max_value_array, min, max);
        print_integer_array(result, min, max);
        exit(EXIT_SUCCESS);
    }
    
    min = ARRAY_SIZE/2;
    max = ARRAY_SIZE;
    calculations(result, array, max_value_array, min, max);
    print_integer_array(result, min, max);
    
    wait(NULL);
    
    free(result);
    free(array);
    exit(EXIT_SUCCESS);
}