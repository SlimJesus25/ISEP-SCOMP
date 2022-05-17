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
#include <semaphore.h>

#define INPUT_FILE "Numbers.txt"
#define OUTPUT_FILE "Output.txt"
#define AMOUNT 1600
#define N_CHILD 8

int main(){

    int num, i=0;

    sem_t *sem; // Semaphore pointer.

    // Creates semaphore and tests it is valid.
    if((sem = sem_open("ex1semph", O_CREAT, 0644, 1)) == SEM_FAILED){
        perror("Error creating semaphore!");
        exit(EXIT_FAILURE);
    }

    // Pointer to manage the input(read) and output(write) file.
    FILE *inputFile, *outputFile;

    pid_t p;

    int min, max;

    // Cycle that will create n new processes.
    for(i=0;i<N_CHILD;i++){
        p = fork();
        min = 200*i;
        max = 200*(i+1);
        if(p == 0)
            break;
    }

    if(p == 0){
        sem_wait(sem);
        //printf("Started: Child %d | PID %d\n", i, getpid());
        inputFile = fopen(INPUT_FILE, "r");
        if(inputFile == NULL){
            perror("Error opening input file!");
            return EXIT_FAILURE;
        }

        int vec[200];
        i=0;
        for(int k=0;k<min;k++){
            fscanf(inputFile, "%d", &num);
        }
        for(int j=min;j<max;j++){
            fscanf(inputFile, "%d", &vec[i++]);
        }

        outputFile = fopen(OUTPUT_FILE, "a");
        printf("Filho %d a ler de %d a %d\n", i, min, max);
        for(int j=0;j<200;j++){
            //fscanf(inputFile, "%d", &num);
            fprintf(outputFile, "%d\n", vec[j]);
        }

        fclose(inputFile);
        fclose(outputFile);

        sem_post(sem);
        //printf("Finished: Child %d | PID %d\n", i, getpid());
        exit(EXIT_SUCCESS);
    }else{
        for(i=0;i<8;i++)
            wait(NULL);
    }

    outputFile = fopen(OUTPUT_FILE, "r");

    if(outputFile == NULL){
        perror("Error opening output file!");
        return EXIT_FAILURE;
    }

    while(fscanf(outputFile, "%d", &num) != EOF)
        printf("%d\n", num);

    fclose(outputFile);

    sem_close(sem);
    sem_unlink("ex1semph");

    return EXIT_SUCCESS;
}