#ifndef SEMAPHORE_UTILS
#define SEMAPHORE_UTILS

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
#include <math.h>

// Valor inicial de um semáforo de exclusão mútua.
#define MUTEX_INITAL_VALUE 1
// Valor inicial de um semáforo de sincronziação de processos.
#define SYNCHRONIZATION_INITAL_VALUE 0

/* *
 * Função que cria semáforos e coloca-os no array passado por parâmetro.
 * Os nomes atribuídos serão "semaphore_name[número_de_criação]" onde os "[]"
   não existem e número de criação é relativo ao valor do incrementador no loop.
 * */
void create_semaphores(sem_t *semaphore_array[], int array_size, char *semaphore_name, int semaphore_inital_value)
{

    for (int i = 0; i < array_size; i++)
    {
        int int_length;
        if (i == 0)
        {
            int_length = 1;
        }
        else
        {
            int_length = log10(i) + 1;
        }
        int length = strlen(semaphore_name) + int_length + 1;

        char name_aux[length];
        snprintf(name_aux, length, "%s%d", semaphore_name, i);

        semaphore_array[i] = sem_open(name_aux, O_CREAT, S_IRUSR | S_IWUSR,
                                      semaphore_inital_value);

        if (semaphore_array[i] == SEM_FAILED)
        {
            perror("sem_open");
            exit(EXIT_FAILURE);
        }
    }
}

/* *
 * Esta função vai desconectar um array de semáforos abertos previamente.
 * É assumido que eles foram criados pela função create_semaphores e só vai funcionar nessa ocasião.
 * */
void unlink_semaphores(char *semaphore_name, int array_size)
{

    for (int i = 0; i < array_size; i++)
    {

        int int_length;
        if (i == 0)
        {
            int_length = 1;
        }
        else
        {
            int_length = log10(i) + 1;
        }

        int length = strlen(semaphore_name) + int_length + 1;

        char name_aux[length];
        snprintf(name_aux, length, "%s%d", semaphore_name, i);
        if (sem_unlink(name_aux) < 0)
        {
            perror("sem_unlink");
            exit(EXIT_FAILURE);
        }
    }
}

/* *
 * Esta função vai desconectar um array de semáforos abertos previamente.
 * */
/*void unlink_semaphores(char** semaphore_name, int array_size){
    for(int i=0;i<array_size;i++){
        if(sem_unlink(sem_name+i) < 0){
            perror("sem_unlink");
            exit(EXIT_FAILURE);
        }
    }
}*/

#endif