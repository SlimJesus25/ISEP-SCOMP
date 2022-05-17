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

typedef struct{
int number;
char name[50];
char address[255];
}struct1;

typedef struct{
struct1 array[100];
}database;


int main(){
	int fd,fd2;

    database *ptr;

    int index=0;

    int *ptr2=&index;

    int data_size=sizeof(database);

    int data_size2=sizeof(index);

    sem_t *sem;

    sem_t *semIndex;



    printf("Insert Application: \n");

    if((sem = sem_open("/ex10Sem", O_CREAT))){
        perror("Error creating semaphore!");
        exit(EXIT_FAILURE);
    }

    if((semIndex = sem_open("/ex10SemIndex", O_CREAT))){
        perror("Error creating semaphore!");
        exit(EXIT_FAILURE);
    }

    fd = shm_open("/shmex10", O_CREAT|O_RDWR, 0);

    ptr = mmap(NULL, data_size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);

    fd2 = shm_open("/shmex10", O_CREAT|O_RDWR, 0);

    ptr2 = mmap(NULL, data_size2, PROT_READ|PROT_WRITE, MAP_SHARED, fd2, 0);

    struct1 insertInfo;

    printf("\nInsert the identification number:");
    scanf("%d",&insertInfo.number);

    printf("\nInsert the name:");
    scanf("%s",insertInfo.name);

    printf("\nInsert the address:");
    scanf("%s",insertInfo.address);

    int atual_index=0;
    sem_wait(semIndex);
    sem_wait(sem);
    atual_index=*ptr2;

    ptr->array[atual_index]=insertInfo;
    
    sem_post(semIndex);
    sem_post(sem);

    sem_close(semIndex);
    sem_close(sem);
    close(fd);
    close(fd2);



	return 0;
}