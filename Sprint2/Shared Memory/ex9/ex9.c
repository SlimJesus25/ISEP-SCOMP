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

typedef struct{
    int arr[10];
    int stop;
}test_t;

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

    int fd, data_size = sizeof(test_t);

    test_t *ptr;

    fd = shm_open("/ex9shrdmem", O_CREAT|O_EXCL|O_RDWR, S_IRUSR|S_IWUSR);
    ftruncate(fd, data_size);
    ptr = (test_t*)mmap(NULL, data_size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
    ptr->stop = 0;

    int index = 0, min_index = 0;
    int max_index = 100;
    for(int i=0;i<CHILD_N;i++){
        if(fork() == 0){
            int local_maximum = largest_number_in_array(array, min_index, max_index);
            printf("%d part\nLocal maximum: %d\n", (i+1), local_maximum);
            ptr->arr[index] = local_maximum;
            exit(EXIT_SUCCESS);
        }
        if(i == CHILD_N-1){
            ptr->stop = 1;
            break;
        }
        min_index = max_index;
        max_index += 100;
        index++;
    }
    while((ptr->stop) == 0);
    int global_maximum = ptr->arr[0];
    for(int i=1;i<CHILD_N;i++){
        if(global_maximum < ptr->arr[i])
            global_maximum = ptr->arr[i];
    }

    printf("Global maximum: %d\n", global_maximum);

        shm_unlink("/ex9shrdmem");

        if(munmap(ptr, data_size)){
            perror("Error disconnecting the shared memory area!\n");
            exit(EXIT_FAILURE);
        }

        if(close(fd) == EXIT_FAILURE){
            perror("Error closing file descriptor!\n");
            exit(EXIT_FAILURE);
        }
    return 0;
}