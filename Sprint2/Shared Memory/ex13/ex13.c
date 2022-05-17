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

/*Implement a program that allows to optimize the search of words in a set of text files in parallel/concurrent
environment.
The parent process should:
• Create an area of shared memory. The memory area must contain, for each child process, the following
information:
– path to the file;
– word to search;
– an integer to store the number of occurrences.
• Create 10 new processes;
• Fill the shared memory area with the information for each child process;
• Wait until the child processes finish their search;
• Print the number of occurrences determined by each child;
• Eliminate the shared memory area.
Each child process should:
• Open the text file assigned to it by the parent (can/should be different for each child process);
• Determine the number of occurrences of the word to search;
• Write the number of occurrences in their position in the shared memory area.*/

#define CHILD_N 10

typedef struct{
    char path[40];
    char word_to_search[15];
    int counter;
    int stop;
}test_t;

int count_word_in_file(char filename[40], char* word_to_count){
    FILE *fptr;
    int count=0;

    fptr = fopen(filename, "r");
    while(fgets(word_to_count, 15, fptr) != EOF){
        count++;
    }
    return count;
}

int main(){

    int fd, data_size = CHILD_N*sizeof(test_t);

    test_t *ptr;

    fd = shm_open("/ex13shrdmem", O_CREAT|O_RDWR, S_IRUSR|S_IWUSR);
    ftruncate(fd, data_size);
    ptr = (test_t*)mmap(NULL, data_size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);

    ptr->stop = 0;

    strcpy(ptr->path, "file_one.txt");
    strcpy(ptr->word_to_search, "aberto");

    strcpy((ptr+1)->path, "file_two.txt");
    strcpy((ptr+1)->word_to_search, "aberto");

    strcpy((ptr+2)->path, "file_three.txt");
    strcpy((ptr+2)->word_to_search, "aberto");

    strcpy((ptr+3)->path, "file_four.txt");
    strcpy((ptr+3)->word_to_search, "aberto");

    strcpy((ptr+4)->path, "file_five.txt");
    strcpy((ptr+4)->word_to_search, "aberto");

    strcpy((ptr+5)->path, "file_six.txt");
    strcpy((ptr+5)->word_to_search, "aberto");

    strcpy((ptr+6)->path, "file_seven.txt");
    strcpy((ptr+6)->word_to_search, "aberto");

    strcpy((ptr+7)->path, "file_eigth.txt");
    strcpy((ptr+7)->word_to_search, "aberto");

    strcpy((ptr+8)->path, "file_nine.txt");
    strcpy((ptr+8)->word_to_search, "aberto");

    strcpy((ptr+9)->path, "file_ten.txt");
    strcpy((ptr+9)->word_to_search, "aberto");

    for(int i=0;i<CHILD_N;i++){
        if(fork() == 0){
            (ptr+i)->counter = count_word_in_file((ptr+i)->path, (ptr+i)->word_to_search);
            if(i+1 == CHILD_N)
                ptr->stop = 1;
            exit(EXIT_SUCCESS);
        }
    }
    while(ptr->stop == 0);
    for(int i=0;i<CHILD_N;i++)
        printf("File: %s | Found %d times the word %s\n", (ptr+i)->path, (ptr+i)->counter, (ptr+i)->word_to_search);

    shm_unlink("/ex13shrdmem");

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