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

/*
Implement a solution that sends the number and name of a student between two processes not related
hierarchically, a writer and a reader.
 - The writer must create a shared memory area, read the data from the keyboard and write them in the shared
memory.
 - The reader should read the data from the shared memory and print them on the screen.
 Note: the writer must be executed before the reader.
*/

typedef struct{
    int number;
    char name[50];
    int active_wait;
}student;

int main(){

    int fd, data_size = sizeof(student);

    student *ptr;

    fd = shm_open("/shrdmmry", O_CREAT|O_EXCL|O_RDWR, S_IRUSR|S_IWUSR);
    ftruncate(fd, data_size);

    ptr = (student *)mmap(NULL, data_size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);

    ptr->active_wait = 0;

    pid_t p = fork();

    if(p < 0){
        perror("Error creating process!");
        return EXIT_FAILURE;
    }

    if(p > 0){
        char student_name[] = "João Teixeira";
        int student_number = 1193091;

        strcpy(ptr->name, student_name);
        ptr->number = student_number;

        ptr->active_wait = 1;
    }else{
        while(!ptr->active_wait);
        printf("Data in shared memory: %s | %d\n", ptr->name, ptr->number);
    }

    if(munmap(ptr, data_size)){
        perror("Error disconnecting the shared memory area!\n");
        exit(EXIT_FAILURE);
    }

    shm_unlink("/shrdmmry");

    return 0;
}