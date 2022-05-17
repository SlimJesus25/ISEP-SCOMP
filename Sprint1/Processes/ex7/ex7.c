#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <time.h>
/*
Given the code below, write the code necessary to create a child process to find how many times the number n is
found in half of the elements of the numbers array. While the child is processing, the parent process should
search the other half. After both finish processing their half, the parent computes and presents how many times n
was found in the entire array (assume that no number is repeated more than 255 times).
*/
#define ARRAY_SIZE 1000

int number_frequency(int number_to_count, int begin_array, int end_array, int *int_array){

    int count=0;

    for(int j=begin_array;j<end_array;j++){
        if(int_array[j] == number_to_count)
            count++;
    }

    return count;
}

int main(){
    int numbers[ARRAY_SIZE], i;

    srand((unsigned)time(NULL));

    for(i=0;i<ARRAY_SIZE;i++)
        numbers[i] = rand() % 100;

    int count, status, n = rand() % 100;;

    pid_t p = fork();

    if(p < 0){
        perror("Error creating child process!");
        return 1;
    }

    if(p == 0){
        count = number_frequency(n, 0, ARRAY_SIZE/2, numbers);
        exit(count);
    }else if(p > 0){
        count = number_frequency(n, ARRAY_SIZE/2, ARRAY_SIZE, numbers);
    }

    wait(&status); // Using wait instead of waitpid because there is only 1 child!

    if(WIFEXITED(status))
        printf("Number of %d occurrences in the entire array: %d\n%d on first half and %d on the other half.", n, (WEXITSTATUS(status) + count), WEXITSTATUS(status), count);

    return 0;
}
