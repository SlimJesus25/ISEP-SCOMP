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
#include <math.h>
#include <pthread.h>

#define FILTERING 3
#define COMPUTING 3
#define THREADS FILTERING + COMPUTING
#define EXAMS 300

typedef struct{
    int number;
    int notaG1;
    int notaG2;
    int notaG3;
    int notaFinal;
}prova_t;

pthread_cond_t cond;
pthread_mutex_t mutex[2];

int positive_exams;
int negative_exams;
int actual_exams;

void* increment_pos(void *arg){

    pthread_mutex_lock(&mutex[0]);
    while(actual_exams < EXAMS){
        pthread_cond_wait(&cond, &mutex[0]);
        actual_exams++;
        positive_exams++;
    }
    pthread_mutex_unlock(&mutex[0]);
    pthread_exit(NULL);
}

void* increment_neg(void *arg){

    pthread_mutex_lock(&mutex[1]);
    while(actual_exams < EXAMS){
        pthread_cond_wait(&cond, &mutex[1]);
        actual_exams++;
        positive_exams++;
    }
    pthread_mutex_unlock(&mutex[1]);
    pthread_exit(NULL);
}

void* generate_exam(void *arg){
    prova_t* array = ((prova_t*)arg);
    for(int i=0;i<EXAMS;i++){
        (array+i)->number = 2023*1000+i;
        (array+i)->notaG1 = rand() % 100;
        (array+i)->notaG2 = rand() % 100;
        (array+i)->notaG3 = rand() % 100;
    }
}

int main(){
    
    srand(time(NULL));

    pthread_t thread[THREADS];

    prova_t* exams = (prova_t*)calloc(EXAMS, sizeof(prova_t));

    pthread_create(thread[0], NULL, generate_exam, ((void*)exams));

    
    

    for(int i=0;i<3;i++)
        pthread_mutex_destroy(&mutex[i]);

    pthread_cond_destroy(&cond);

    pthread_exit(NULL);
}