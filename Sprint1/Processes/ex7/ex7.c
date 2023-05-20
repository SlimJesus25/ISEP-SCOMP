#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <time.h>

#define ARRAY_SIZE 1000

int search_frequency(int* array, int number_to_search, int min_limit, int max_limit){

    int frequency = 0;

    for(int i=min_limit;i<max_limit;i++){
        if(array[i] == number_to_search){
            frequency++;
        }
    }
    return frequency;
}

void print_array(int* array, int size){
    for(int i=0;i<size;i++){
        printf("%d|", array[i]);
    }
}

int main(){
    int numbers[ARRAY_SIZE]; /* array to lookup */
    int n; /* the number to find */
    time_t t; /* needed to init. the random number generator (RNG)
    */
    int i;

    /* intializes RNG (srand():stdlib.h; time(): time.h) */
    srand ((unsigned) time (&t));

    /* initialize array with random numbers (rand(): stdlib.h) */
    for (i = 0; i < ARRAY_SIZE; i++)
        numbers[i] = rand () % 100 ;

    /* initialize n */
    n = rand () % 100;

    int frequency, status;

    //print_array(numbers, ARRAY_SIZE);

    if(fork() == 0){
        frequency = search_frequency(numbers, n, 0, ARRAY_SIZE/2);
        printf("\nFound %d times in first half!\n", frequency);
        exit(frequency);
    }

    frequency = search_frequency(numbers, n, ARRAY_SIZE/2, ARRAY_SIZE);
    printf("\nFound %d times in second half!\n", frequency);
    wait(&status);

    frequency += WEXITSTATUS(status);
    printf("\nFrequency of %d: %d\n", n, frequency);
    return 0;
}