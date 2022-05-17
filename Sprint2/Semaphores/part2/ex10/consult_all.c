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

	printf("Consult ALl Application: \n");

	if((sem = sem_open("/ex10Sem", O_CREAT | O_EXCL, 0644,1))){
        perror("Error creating semaphore!");
        exit(EXIT_FAILURE);
    }

    if((semIndex = sem_open("/ex10SemIndex", O_CREAT | O_EXCL, 0644,1))){
        perror("Error creating semaphore!");
        exit(EXIT_FAILURE);
    }


    fd = shm_open("/shmex10", O_CREAT|O_EXCL|O_RDWR, S_IRUSR|S_IWUSR);
    ftruncate(fd, data_size);

    ptr = mmap(NULL, data_size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);

    fd2 = shm_open("/shmex10Index", O_CREAT|O_EXCL|O_RDWR, S_IRUSR|S_IWUSR);
    ftruncate(fd, data_size2);

    ptr2 = mmap(NULL, data_size2, PROT_READ|PROT_WRITE, MAP_SHARED, fd2, 0);

    int j=0;

    
    while(j==0){
    	printf("Type 1 to close the App \n");
    printf("Type 0 to consult all Data \n");
    	scanf("%d",&j);
    	if(j==0){
    		sem_wait(semIndex);
    		sem_wait(sem);
    		int current_index=*ptr2;
    		for(int i=0;i<current_index;i++){
    			struct1 person = ptr->array[i];
    			printf("Number: %i, Name: %s, Address: %s \n",person.number,person.name,person.address);
    		}
    		sem_post(semIndex);
    		sem_post(sem);
    	}
    }

    printf("Closing");

    shm_unlink("/shmex10");
    shm_unlink("/shmex10Index");
    sem_unlink("/ex10SemIndex");
    sem_unlink("/ex10Sem");
	
	return 0;
}