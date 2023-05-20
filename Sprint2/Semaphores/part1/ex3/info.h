#ifndef INFO_H
#define INFO_H

#define MUTEX_NAME "/ex2_mutex"
#define MUTEX_INITAL_VALUE 1
#define SHARED_MEMORY_NAME "/ex2_shrdmem"
#define STRING_CHARACTER_NUMBER 80
#define STRING_QUANTITY 50
#define BARRIER_NAME "/exe2_barrier"
#define SYNC_INITAL_VALUE 0

typedef struct
{
    char array_strings[STRING_QUANTITY][STRING_CHARACTER_NUMBER];
    int iteration;
    int children_counter;
} info_t;

#endif