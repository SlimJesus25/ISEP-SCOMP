#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <signal.h>
#include <sys/time.h>

/*
   Implement a program to determine the biggest element of an array in a parallel/concurrent environment. The
   parent process should:
   • Create an array of 1000 integers, initializing it with random values between 0 and 1000;
   • Create a shared memory area to store an array of 10 integers, each containing the local maximum of 100 values;
   • Create 10 new processes;
   • Wait until the 10 child processes finish the search for the local maximum;
   • Determine the global maximum;
   • Eliminate the shared memory area.
   Each child process should:
   • Calculate the largest element in the 100 positions;
   • Write the value found in the position corresponding to the order number (0-9) in the array of local maximum.
   */

#define ARRAY_SIZE 1000
#define CHILD_N 10

int largest_number_in_array(int* array, int minimumIndex, int maximumIndex){
    int largest_number = array[minimumIndex];
    for(int j=minimumIndex+1; j<maximumIndex;j++){
        if(largest_number < array[j])
            largest_number = array[j];
    }
    return largest_number;
}

int main(){

    int array[ARRAY_SIZE];

    srand((unsigned) time(NULL));

    for(int i=0;i<ARRAY_SIZE;i++)
        array[i] = rand() % 1001;

    int fd[CHILD_N][2];

    for(int i=0;i<CHILD_N;i++){ // Creation of 5 pipes
        if(pipe(fd[i]) == -1){
            perror("Pipe failed");
            return 1;
        }
    }

    int index = 0, min_index = 0;
    int max_index = 100;

    int *local_maximum_sent = (int*)calloc(1, sizeof(int));
    int *local_maximum_received = (int*)calloc(1, sizeof(int));

    for(int i=0;i<CHILD_N;i++){
        if(fork() == 0){
            close(fd[i][0]);
            *local_maximum_sent = largest_number_in_array(array, min_index, max_index);
            printf("%d part\nLocal maximum: %d\n", (i+1), *local_maximum_sent);
            write(fd[i][1], local_maximum_sent, sizeof(local_maximum_sent));
            close(fd[i][1]);
            exit(EXIT_SUCCESS);
        }
        min_index = max_index;
        max_index += 100;
        index++;
    }

    int global_maximum = 0;
    for(int i=0;i<CHILD_N;i++){
        close(fd[i][1]);
        read(fd[i][0], local_maximum_received, sizeof(local_maximum_received));
        if(*local_maximum_received > global_maximum)
            global_maximum = *local_maximum_received;
        close(fd[i][0]);
    }

    printf("Global maximum: %d\n", global_maximum);

    return 0;
}