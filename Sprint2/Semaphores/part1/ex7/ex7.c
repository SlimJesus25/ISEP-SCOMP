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
#include <math.h>

#define SEMAPHORES 3
#define CHILD_NUMBER 3

int main(){

    sem_t *sem_r;
    if((sem_r = sem_open("ex7semR", O_CREAT, 0644, 3)) == SEM_FAILED){
        perror("Error creating semaphore!");
        exit(EXIT_FAILURE);
    }

    /*int fd;

    fd = shm_open("/ex7sem_shmd", O_CREAT|O_RDWR, S_IRUSR|S_IWUSR);
    ftruncate(fd, sizeof(sem_t));
    sem_r = (sem_t*)mmap(NULL, sizeof(sem_t), PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);*/

    sem_t **sem;
    //txt1 = (char**)calloc(12, sizeof(char));
    char txt1[2][12] = {"Sistemas ", "a "};
    //txt2 = (char**)calloc(11, sizeof(char));
    char txt2[2][11] = {"de ", "melhor "};
    //txt3 = (char**)calloc(28, sizeof(char));
    char txt3[2][28] = {"Computadores - ", "disciplina! "};

    sem = (sem_t**)calloc(SEMAPHORES, sizeof(sem_t));

    char *str;

    /*int **teste;

    int *teste1, *teste2;

    teste = (int**)calloc(6, sizeof(int));

    teste1 = (int*)calloc(3, sizeof(int));

    teste2 = (int*)calloc(3, sizeof(int));

    *teste1 = 10;
    *(teste1+1) = 20;
    *(teste1+2) = 30;

    *teste2 = 15;
    *(teste2+1) = 25;
    *(teste2+2) = 3;

    *teste = teste1;
    *(teste+1) = teste2;

    printf("Aqui: %d\n", *(*(teste)+1));
    printf("Aqui2: %d\n", *(teste+1));

    for(int i=0;i<2;i++){
        for(int j=0;j<3;j++){
            printf("%d | ", *(*(teste+i)+j));
        }
    }

    teste_func(*(teste+1));*/
    int r;
    str = (char*)calloc(20, sizeof(char));
    for(int i=0;i<SEMAPHORES;i++){
        snprintf(str, 20, "ex7sem%d", (i+1));
        //printf("%s\n", str);
        if((*(sem+i) = sem_open(str, O_CREAT, 0644, 1)) == SEM_FAILED){
            perror("Error creating semaphore!");
            exit(EXIT_FAILURE);
        }
    }
    free(str);
    pid_t p;

    int i;

    for(i=0;i<CHILD_NUMBER;i++){
        if((p = fork()) == 0){
            break;
        }
    }
    if(p == 0){

        sem_wait(*(sem+i));
        if(i==0){
            printf("%s\n", txt1[0]);
            sem_wait(sem_r);
        }
         else if(i==1){
            printf("%s\n", txt2[0]);
            sem_wait(sem_r);
         }
         else if(i==2){
            printf("%s\n", txt3[0]);
            sem_wait(sem_r);
         }

         while(r > 0){
         printf("Valor de r: %d\n", r);
            sem_getvalue(sem_r, &r);
         }
        sem_post(*(sem+i));

        if(i==0)
            printf("%s", txt1[1]);
         else if(i==1)
            printf("%s", txt2[1]);
         else
            printf("%s", txt3[1]);
    }

    str = (char*)calloc(20, sizeof(char));

    for(i=0;i<SEMAPHORES;i++){
        sem_close(*(sem+i));
        snprintf(str, 20, "ex7sem%d", (i+1));
        sem_unlink(str);
    }


    return 0;
}