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

int main(){

    int* array = (int*) calloc(ARRAY_SIZE, sizeof(int));
    
    generate_random_integer_array(array, RANGE_MINIMUM, RANGE_MAXIMUM);

    return 0;
}