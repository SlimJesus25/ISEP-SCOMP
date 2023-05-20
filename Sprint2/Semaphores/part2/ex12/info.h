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
#include <stdbool.h>

#define BUFFER_SIZE 10
#define TOTAL_INTEGERS 30
#define CHILD_NUM 2
#define SHARED_MEMORY_NAME "/exercicio12"
#define SEMAPHORE_MUTEX "/exercicio12-mutex"
#define SEMAPHORE_WRITE "/exercicio12-write"
#define SEMAPHORE_READ "/exercicio12-read"

#define SEMAPHORE_INITAL_VALUE_1 1
#define SEMAPHORE_INITAL_VALUE_0 0
typedef struct
{
    int data[BUFFER_SIZE];
    int head;
    int tail;

} circular_buffer_t;

#endif