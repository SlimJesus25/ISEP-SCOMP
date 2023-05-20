#ifndef INFO_H
#define INFO_H

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

#define STR_SIZE 80
#define MAX_CLIENTS 100
#define SHARED_MEMORY_NAME "/exercicio8"
#define SEMAPHORE_INITAL_VALUE_1 1
#define SEMAPHORE_INITAL_VALUE_0 0
#define SEMAPHORE_NAME_1 "/exercicio8-1"
#define SEMAPHORE_NAME_2 "/exercicio8-2"
#define SEMAPHORE_NAME_3 "/exercicio8-3"

typedef struct
{

    int number;
    char name[STR_SIZE];
    char address[STR_SIZE];

} info_t;

typedef struct
{
    int number_of_clients;
    info_t info_array[MAX_CLIENTS];

} shared_data_t;
/*
 void fill_info(info_t *shared_info, int i)
 {
     shared_info->number = i;
     snprintf(shared_info->name, STR_SIZE, "cliente_%d", i);
     snprintf(shared_info->address, STR_SIZE, "rua_%d", i);
 }
*/

#endif
