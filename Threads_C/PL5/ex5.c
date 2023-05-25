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

#define THREADS 3
#define ARRAY_SIZE 1000

int glowest_balence;
int ghighest_balance;
float gaverage_balance;

void find_lowest_balance(void* arg){
    int* balance = (int*)arg;
    int lowest_balance = *(balance);
    for(int i=1;i<ARRAY_SIZE;i++){
        if(*(balance+i) < lowest_balance){
            lowest_balance = *(balance+i);
        }
    }
    glowest_balence = lowest_balance;
    pthread_exit((void*)NULL);
}

void find_highest_balance(void* arg){
    int* balance = (int*)arg;
    int highest_balance = *(balance);
    for(int i=1;i<ARRAY_SIZE;i++){
        if(*(balance+i) > highest_balance){
            highest_balance = *(balance+i);
        }
    }
    ghighest_balance = highest_balance;
    pthread_exit((void*)NULL);
}

void find_average_balance(void* arg){
    int* balance = (int*)arg;
    float avg = 0;
    for(int i=0;i<ARRAY_SIZE;i++){
        avg += *(balance+i);
    }
    gaverage_balance = avg/ARRAY_SIZE;
    
    pthread_exit((void*)NULL);
}

int main(){

    srand(time(NULL));

    pthread_t threads[THREADS];

    int* balances = (int*)calloc(ARRAY_SIZE, sizeof(int));

    for(int i=0;i<ARRAY_SIZE;i++){
        *(balances+i) = rand()%5000;
    }
        
    pthread_create(&threads[0], NULL, (void*)find_lowest_balance, (void*)balances);
    pthread_create(&threads[1], NULL, (void*)find_highest_balance, (void*)balances);
    pthread_create(&threads[2], NULL, (void*)find_average_balance, (void*)balances);
    
    for(int i=0;i<THREADS;i++){
        pthread_join(threads[i], NULL);
    }

    printf("Highest value: %d\nLowest value: %d\nAverage balance: %.2f\n", ghighest_balance, glowest_balence, gaverage_balance);
    
    
    pthread_exit((void*)NULL);

}