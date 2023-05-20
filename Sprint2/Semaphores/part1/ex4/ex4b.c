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

#define SEMAPHORE_INITAL_VALUE_0 0
#define SEMAPHORE_INITAL_VALUE_1 1
#define SEMAPHORE_NAME_1 "/exercicio4b-1"
#define SEMAPHORE_NAME_2 "/exercicio4b-2"
#define SEMAPHORE_NAME_3 "/exercicio4b-3"
#define MSG_NUMBER 15

int main(int argc, char *argv[])
{

    /*
    Neste exercício foi usado 3 semáforos de sincronização de processos
    */
    pid_t p1, p2;

    sem_t *proc_sync = sem_open(SEMAPHORE_NAME_1, O_CREAT | O_EXCL, 0644, SEMAPHORE_INITAL_VALUE_1);
    sem_t *proc_sync2 = sem_open(SEMAPHORE_NAME_2, O_CREAT | O_EXCL, 0644, SEMAPHORE_INITAL_VALUE_0);
    sem_t *proc_sync3 = sem_open(SEMAPHORE_NAME_3, O_CREAT | O_EXCL, 0644, SEMAPHORE_INITAL_VALUE_0);

    if (proc_sync == SEM_FAILED || proc_sync2 == SEM_FAILED || proc_sync3 == SEM_FAILED)
    {
        perror("sem_open");
        exit(EXIT_FAILURE);
    }

    p1 = fork();

    if (p1 < 0)
    {
        perror("fork 1");
        exit(EXIT_FAILURE);
    }
    if (p1 > 0) // father overrall
    {
        for (int i = 0; i < MSG_NUMBER; i++)
        {
            sem_wait(proc_sync2);
            printf("Father \n");
            sem_post(proc_sync3);
        }
    }
    else if (p1 == 0) // child 1
    {
        int status;
        p2 = fork(); // child 1 forks creates the child 2
        if (p2 < 0)
        {
            perror("fork 2");
            exit(EXIT_FAILURE);
        }
        if (p2 > 0) // child 1 overral
        {
            for (int i = 0; i < MSG_NUMBER; i++)
            {
                sem_wait(proc_sync);
                printf("1st Child \n");
                sem_post(proc_sync2);
            }
        }
        else if (p2 == 0) // child 2, created by child 1
        {
            for (int i = 0; i < MSG_NUMBER; i++)
            {
                sem_wait(proc_sync3);
                printf("2nd Child \n");
                sem_post(proc_sync);
            }
            exit(EXIT_SUCCESS);
        }
        wait(&status);
        exit(EXIT_SUCCESS);
    }

    if (sem_unlink(SEMAPHORE_NAME_1) < 0)
    {
        perror("sem_unlink 1");
    }
    if (sem_unlink(SEMAPHORE_NAME_2) < 0)
    {
        perror("sem_unlink 2");
    }
    if (sem_unlink(SEMAPHORE_NAME_3) < 0)
    {
        perror("sem_unlink 3");
    }

    exit(EXIT_SUCCESS);
}