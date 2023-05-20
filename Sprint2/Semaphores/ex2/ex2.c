#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/syscall.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <sys/wait.h>
#include <ctype.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <semaphore.h>

#define SEMAPHORE_NAME "/my_semaphore"
#define SEMAPHORE_WRITE_NAME "/my_write_semaphore"
#define SEMAPHORE_INITAL_VALUE 1

#define NUMBER_PER_CHILDREN 200
#define CHILDREN 8
#define NUMBERS_FILE "Numbers.txt"
#define OUTPUT_FILE "Output.txt"

int main()
{

    pid_t p;
    sem_t *semaphores[CHILDREN];
    char name[80];

    for (int i = 0; i < CHILDREN; i++)
    {
        snprintf(name, 80, "%s_%d", SEMAPHORE_NAME, i);
        semaphores[i] = sem_open(name, O_CREAT | O_EXCL, 0644, SEMAPHORE_INITAL_VALUE);

        if (semaphores[i] == SEM_FAILED)
        {
            perror("sem_open");
            exit(EXIT_FAILURE);
        }
    }

    sem_t *semaphore_write = sem_open(SEMAPHORE_WRITE_NAME, O_CREAT | O_EXCL, 0644, SEMAPHORE_INITAL_VALUE);

    if (semaphore_write == SEM_FAILED)
    {
        perror("sem_open");
        exit(EXIT_FAILURE);
    }

    for (int i = 1; i < CHILDREN; i++)
    {
        sem_wait(semaphores[i]);
    }

    /*
    for (int i = 0; i < CHILDREN; i++)
    {
        int value;
        sem_getvalue(semaphores[i], &value);
        printf("semaphore %d value: %d \n", i, value);
    }
    */

    for (int i = 0; i < CHILDREN; i++)
    {

        p = fork();

        if (p < 0)
        {
            perror("fork");
            exit(EXIT_FAILURE);
        }

        if (p == 0)
        {

            sem_wait(semaphores[i]);

            FILE *fptr = fopen(NUMBERS_FILE, "r");
            printf("Abri o ficheiro\n");

            if (fptr == NULL)
            {
                perror("fopen 1");
                exit(EXIT_FAILURE);
            }

            // Variável que vai armazenando os valores do ficheiro.
            char number[10];
            // Array de inteiros que vai armazenar os valores a escrever no ficheiro "Output.txt".
            int read_values[NUMBER_PER_CHILDREN], count = 0;

            while (fscanf(fptr, "%s", number) != EOF && count < NUMBER_PER_CHILDREN)
            {
                read_values[count] = atoi(number);
                count++;
            }

            fclose(fptr);

            sem_post(semaphores[i + 1]);

            // sem_post(semaphores[i + 1]);

            // writing access part
            sem_wait(semaphore_write);

            FILE *fptr_w = fopen(OUTPUT_FILE, "a");

            if (fptr_w == NULL)
            {
                perror("fopen 2");
                exit(EXIT_FAILURE);
            }

            for (int i = 0; i < NUMBER_PER_CHILDREN; i++)
            {
                fprintf(fptr_w, "[%d] %d\n", getpid(), read_values[i]);
            }

            fclose(fptr_w);

            sem_post(semaphore_write);

            exit(EXIT_SUCCESS);
        }
    }

    for (int i = 0; i < CHILDREN; i++)
    {
        wait(NULL);
    }

    FILE *fptr = fopen(OUTPUT_FILE, "r");

    if (fptr == NULL)
    {
        perror("fopen 3");
        exit(EXIT_FAILURE);
    }

    char output[50];
    int i = 0;
    while (fscanf(fptr, "%s", output) != EOF)
    {
        printf("%s", output);
        if (i++ % 2 != 0)
        {
            printf("\n");
        }
    }

    if (sem_unlink(SEMAPHORE_WRITE_NAME) < 0)
    {
        perror("sem_unlink write");
    }

    for (int i = 0; i < CHILDREN; i++)
    {
        snprintf(name, 80, "%s_%d", SEMAPHORE_NAME, i);

        if (sem_unlink(name) < 0)
        {
            perror("sem_unlink");
        }
    }

    // Remove o semáforo.
    // const char* name -> Nome do semáforo a remover.

    exit(EXIT_SUCCESS);
}