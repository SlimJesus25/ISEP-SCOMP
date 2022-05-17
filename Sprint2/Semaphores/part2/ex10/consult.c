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
	int fd;

    database *ptr;

    int data_size=sizeof(database);

    sem_t *sem;

    int wanted_n;

    printf("Consult  Application: \n");

	if((sem = sem_open("/ex10Sem", O_CREAT))){
        perror("Error creating semaphore!");
        exit(EXIT_FAILURE);
    }

    fd = shm_open("/shmex10", O_CREAT|O_RDONLY, 0);

    ptr = mmap(NULL, data_size, PROT_READ, MAP_SHARED, fd, 0);

    printf("Insert the identification number for consulting");
    scanf("%d",&wanted_n);

    sem_wait(sem);

    for(int i=0;i<100;i++){
    	    			struct1 person = ptr->array[i];
    			
    	if(wanted_n == person.number){
			printf("Number: %i, Name: %s, Address: %s \n",person.number,person.name,person.address);
    	}
    }

    sem_post(sem);

    sem_close(sem);

    close(fd);

	return 0;
}