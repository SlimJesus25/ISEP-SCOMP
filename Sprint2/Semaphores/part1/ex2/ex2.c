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

    sem_t *semaphores[N_CHILD];


    if (
    (semaphores[0] = sem_open("ex1sem1", O_CREAT, 0644,1)) == SEM_FAILED||
    (semaphores[1] = sem_open("ex1sem2", O_CREAT, 0644,0)) == SEM_FAILED||
    (semaphores[2] = sem_open("ex1sem3", O_CREAT, 0644,0)) == SEM_FAILED||
    (semaphores[3] = sem_open("ex1sem4", O_CREAT, 0644,0)) == SEM_FAILED||
    (semaphores[4] = sem_open("ex1sem5", O_CREAT, 0644,0)) == SEM_FAILED||
    (semaphores[5] = sem_open("ex1sem6", O_CREAT, 0644,0)) == SEM_FAILED||
    (semaphores[6] = sem_open("ex1sem7", O_CREAT, 0644,0)) == SEM_FAILED||
    (semaphores[7] = sem_open("ex1sem8", O_CREAT, 0644,0)) == SEM_FAILED) 
    {
        perror("Error creating semaphore!");
        exit(EXIT_FAILURE);
    }

    int num, i=0;

    sem_t *sem; // Semaphore pointer.

    // Creates semaphore and tests it is valid.
    if((sem = sem_open("ex1sem", O_CREAT|O_EXCL, 0644, 1)) == SEM_FAILED){
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

        if(p == 0){
        sem_wait(semaphores[i]);
        //printf("Started: Child %d | PID %d\n", i, getpid());

        inputFile = fopen(INPUT_FILE, "r");
        if(inputFile == NULL){
            perror("Error opening input file!");
            return EXIT_FAILURE;
        }

        outputFile = fopen(OUTPUT_FILE, "a");
        printf("Filho %d a ler de %d a %d\n", i, min, max);
        for(int j=min;j<max;j++){
            fscanf(inputFile, "%d", &num);
            printf("NUM: %d",num);
            fprintf(outputFile, "%d\n", num);
        }

        fclose(inputFile);
        fclose(outputFile);
        if(i<7){
         sem_post(semaphores[i+1]);   
        }
        
        //printf("Finished: Child %d | PID %d\n", i, getpid());
        exit(EXIT_SUCCESS);
    }
    }

    if(p > 0){
        for(i=0;i<8;i++)
            wait(NULL);
    }

    outputFile = fopen(OUTPUT_FILE, "r");

    if(outputFile == NULL){
        perror("Error opening output file!");
        return EXIT_FAILURE;
    }

    while(fscanf(outputFile, "%d", &num) != EOF)
        //printf("%d\n", num);

    fclose(outputFile);

    sem_close(sem);
    sem_unlink("ex1sem");

    sem_unlink("ex1sem1");
    sem_unlink("ex1sem2");
    sem_unlink("ex1sem3");
    sem_unlink("ex1sem4");
    sem_unlink("ex1sem5");
    sem_unlink("ex1sem6");
    sem_unlink("ex1sem7");
    sem_unlink("ex1sem8");


    return EXIT_SUCCESS;
}