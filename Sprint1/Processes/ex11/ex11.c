#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <time.h>
/*
Write a program that initializes an array numbers with 1000 random integers in the range [0,255]. The program
should:
*/
#define ARRAY_SIZE 1000

int maximum_value_array(int min, int max, int *arr){
    int maximum_value = arr[0];
    for(int k=min;k<max;k++){
        if(arr[k] > maximum_value){
            maximum_value = arr[k];
        }
    }
    return maximum_value;
}

void calculation_array(int min, int max, int *arr, int *numbers, int maximum_value_entire_array){
    for(int j=min;j<max;j++){
        arr[j] = ((int)numbers[j]/maximum_value_entire_array)*100;
        printf("Index [%d]: %d\n", j+1, arr[j]);
    }
}

int main(){
    int numbers[ARRAY_SIZE];
    int n, status, i;

    srand((unsigned)time(NULL));

    for(i=0;i<ARRAY_SIZE;i++)
        numbers[i] = rand() % 256;

    int maximum_value=numbers[0];

    pid_t p[5];

    for(int j=0;j<5;j++){ // This cycle will create 5 children to concurrently find the maximum value of each 1/5 of the array.
        p[j] = fork(); // Vector storing the childs processes PIDs.
        if(p[j] == 0){
            int maximum_value = maximum_value_array(j*200, (j+1)*200, numbers);
	        printf("\nMaximum value [%d, %d] [FOUND BY %d]: %d\n", j*200, (j+1)*200, getpid(), maximum_value);
	        exit(maximum_value);
        }
    }

    int maximum_value_entire_array = 0;
    for(i=0;i<5;i++){
        waitpid(p[i], &status, 0); // Waits for every child process to finish and then it will find the maximum value of the entire array.
        if(maximum_value_entire_array < WEXITSTATUS(status))
            maximum_value_entire_array = WEXITSTATUS(status);
    }
    printf("\nMaximum value found on the entire array: %d\n\n", maximum_value_entire_array);

    int result[ARRAY_SIZE];

    if(fork() == 0){ // It's very hard to make this 2 processes run concurrently, thus we use wait(NULL) to make parent process wait for the child one to finish.
        calculation_array(0, ARRAY_SIZE/2, result, numbers, maximum_value_entire_array);
        exit(1);
    }else{
        wait(NULL);
        calculation_array(ARRAY_SIZE/2, ARRAY_SIZE, result, numbers, maximum_value_entire_array);
    }
}
