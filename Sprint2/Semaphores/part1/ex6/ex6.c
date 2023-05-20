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

#define SEMAPHORE_NAME_1 "/exercicio8a"
#define SEMAPHORE_NAME_2 "/exercicio8b"
#define SEMAPHORE_INITAL_VALUE 1
#define MAX 10

int main()
{

    pid_t p;

    sem_t *proc_sync1 = sem_open(SEMAPHORE_NAME_1, O_CREAT | O_EXCL, 0644, SEMAPHORE_INITAL_VALUE);
    sem_t *proc_sync2 = sem_open(SEMAPHORE_NAME_2, O_CREAT | O_EXCL, 0644, SEMAPHORE_INITAL_VALUE);

    if (proc_sync1 == SEM_FAILED)
    {
        perror("sem_open");
        exit(EXIT_FAILURE);
    }

    p = fork();

    if (p < 0)
    {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (p == 0)
    {

        for (int i = 0; i < MAX; i++)
        {
            sem_wait(proc_sync1);
            printf("C");
            fflush(stdout);
            sem_post(proc_sync2);
        }
        exit(EXIT_SUCCESS);
    }
    else if (p > 0)
    {
        for (int i = 0; i < MAX; i++)
        {

            sem_wait(proc_sync2);
            printf("S");
            fflush(stdout);
            sem_post(proc_sync1);
        }
    }

    if (sem_unlink(SEMAPHORE_NAME_1) < 0)
    {
        perror("sem_unlink 1");
        exit(EXIT_FAILURE);
    }
    if (sem_unlink(SEMAPHORE_NAME_2) < 0)
    {
        perror("sem_unlink 2");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}