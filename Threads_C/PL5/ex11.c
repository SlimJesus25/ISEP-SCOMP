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

#define EXAMS 300
#define THREADS 5

typedef struct{
    int number;
    int notaG1;
    int notaG2;
    int notaG3;
    int notaFinal;
}prova_t;

pthread_cond_t cond[4];
pthread_mutex_t mutex[4];

prova_t exams[EXAMS];
int positive_exams;
int negative_exams;
int actual_exams;
int ticket;
int ticket2;
int ticket3;

void* increment_pos(void *arg){

    while(1){
        pthread_mutex_lock(&mutex[1]);
        while(ticket2 < 1)
            pthread_cond_wait(&cond[1], &mutex[1]);
        
        pthread_mutex_lock(&mutex[3]);
        actual_exams++;
        pthread_mutex_unlock(&mutex[3]);
        
        positive_exams++;
        ticket2--;
        pthread_mutex_unlock(&mutex[1]);
        pthread_cond_signal(&cond[3]);
    }
    
    pthread_exit(NULL);
}

void* increment_neg(void *arg){
    
    while(1){
        pthread_mutex_lock(&mutex[2]);
        while(ticket3 < 1)
            pthread_cond_wait(&cond[2], &mutex[2]);
        
        pthread_mutex_lock(&mutex[3]);
        actual_exams++;
        pthread_mutex_unlock(&mutex[3]);
        
        negative_exams++;
        ticket3--;
        pthread_mutex_unlock(&mutex[2]);
        pthread_cond_signal(&cond[3]);
    }
    
    pthread_exit(NULL);
}

void* final_grade_calculator(void* arg){
    
    while(1){
        
        pthread_mutex_lock(&mutex[0]);
        while(ticket < 1)
            pthread_cond_wait(&cond[0], &mutex[0]);
        
        exams[actual_exams].notaFinal = (exams[actual_exams].notaG1 
        + exams[actual_exams].notaG3 + exams[actual_exams].notaG2)/3;
        //printf("Final grade: %d\n", exams[actual_exams].notaFinal);
        
        if(exams[actual_exams].notaFinal >= 50){
            pthread_mutex_lock(&mutex[1]);
            ticket2++;
            pthread_mutex_unlock(&mutex[1]);
            pthread_cond_signal(&cond[1]);
        }else{
            pthread_mutex_lock(&mutex[2]);
            ticket3++;
            pthread_mutex_unlock(&mutex[2]);
            pthread_cond_signal(&cond[2]);
        }
            
        ticket--;
        pthread_mutex_unlock(&mutex[0]);
        
        
    }
    
    pthread_exit(NULL);
}

void* generate_exam(void *arg){

    for(int i=0;i<EXAMS;i++){

        exams[i].number = 2023*1000+i;
        exams[i].notaG1 = rand() % 100;
        exams[i].notaG2 = rand() % 100;
        exams[i].notaG3 = rand() % 100;
        
        // printf("Generated exam\nN1: %d\nN2: %d\nN3: %d\nStud. Num. %d\n\n",
        // exams[i].notaG1, exams[i].notaG2, exams[i].notaG3, exams[i].number);
        
        pthread_mutex_lock(&mutex[0]);
        ticket++;
        pthread_mutex_unlock(&mutex[0]);

        // Notifies threads T2 and T3.
        pthread_cond_signal(&cond[0]);
    }
    
}

int main(){
    
    srand(time(NULL));
    
    positive_exams = negative_exams = actual_exams 
    = ticket = ticket2 = ticket3 = 0;
    
    for(int i=0;i<3;i++){
        pthread_cond_init(&cond[i], NULL);
    }
    
    for(int i=0;i<4;i++){
        pthread_mutex_init(&mutex[i], NULL);
    }

    pthread_t thread[THREADS];

    for(int i=1;i<=THREADS-1;i++){
        if(i <= 2)
            pthread_create(&thread[i], NULL, final_grade_calculator, NULL);
        else if(i == 3)
            pthread_create(&thread[i], NULL, increment_pos, NULL);
        else
            pthread_create(&thread[i], NULL, increment_neg, NULL);
    }

    pthread_create(&thread[0], NULL, generate_exam, NULL);

    pthread_join(thread[0], NULL);
    
    pthread_mutex_lock(&mutex[3]);
    while(actual_exams < EXAMS)
        pthread_cond_wait(&cond[3], &mutex[3]);
    pthread_mutex_unlock(&mutex[3]);
    
    float pos = (float)positive_exams/EXAMS;
    float neg = (float)negative_exams/EXAMS;
    
    printf("\tStatistics\nPositive exams: %.2f\nNegative exams: %.2f\n", pos*100, neg*100);

    exit(EXIT_SUCCESS);

    pthread_exit(NULL);
}