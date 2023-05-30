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
#define ARRAY_SIZE 10000

typedef struct{
    int id_h;
    int id_p;
    int p;
}study_t;

study_t vec[ARRAY_SIZE];

study_t vec1[ARRAY_SIZE];
int vec1_index;

study_t vec2[ARRAY_SIZE];
int vec2_index;

study_t vec3[ARRAY_SIZE];
int vec3_index;

int filter;
int compute;

study_t lowest_cost[3];

pthread_cond_t cond;
pthread_mutex_t mutex[3];

void* filtering(void *arg){

    int index = *((int*)arg);

    for(int i=(ARRAY_SIZE/FILTERING*index);i<ARRAY_SIZE/FILTERING*(index+1);i++){
        switch(vec[i].id_h){
            case 0:
                pthread_mutex_lock(&mutex[0]);
                vec1[vec1_index].id_h = vec[i].id_h;
                vec1[vec1_index].id_p = vec[i].id_p;
                vec1[vec1_index].p = vec[i].p;
                vec1_index++;
                pthread_mutex_unlock(&mutex[0]);
                break;
            case 1:
                pthread_mutex_lock(&mutex[1]);
                vec2[vec2_index].id_h = vec[i].id_h;
                vec2[vec2_index].id_p = vec[i].id_p;
                vec2[vec2_index].p = vec[i].p;
                vec2_index++;
                pthread_mutex_unlock(&mutex[1]);
                break;
            case 2:
                pthread_mutex_lock(&mutex[2]);
                vec3[vec3_index].id_h = vec[i].id_h;
                vec3[vec3_index].id_p = vec[i].id_p;
                vec3[vec3_index].p = vec[i].p;
                vec3_index++;
                pthread_mutex_unlock(&mutex[2]);
                break;
        }
    }

    pthread_mutex_lock(&mutex[0]);
    filter++;
    if(filter == FILTERING){
        compute++;
        pthread_cond_signal(&cond);
    }
    pthread_mutex_unlock(&mutex[0]);
    pthread_exit(NULL);
}

void* computing(void *arg){

    study_t* supermarket = (*((int*)arg) == 4) ? vec1 : (*((int*)arg) == 5) ? vec2 : vec3;
    
    pthread_mutex_lock(&mutex[0]);
    while(compute < 1){
        pthread_cond_wait(&cond, &mutex[0]);
    }
    pthread_mutex_unlock(&mutex[0]);
    pthread_cond_signal(&cond);
    
    int prod_avg_prices[5][1];
    int prod_quantities[5];

    for(int i=0;i<ARRAY_SIZE;i++){
        prod_avg_prices[(supermarket+i)->id_p][0] += (supermarket+i)->p;
        prod_quantities[(supermarket+i)->id_p]++;
    }

    int total_price = 0;

    for(int i=0;i<5;i++){
        prod_avg_prices[i][0] /= prod_quantities[i];
        total_price += prod_avg_prices[i][0];
    }

    if(*((int*)arg) == 4){
        lowest_cost[0].id_h = 0;
        lowest_cost[0].id_p = total_price;
        lowest_cost[0].p = -1;
    }else if(*((int*)arg) == 5){
        lowest_cost[1].id_h = 1;
        lowest_cost[1].id_p = total_price;
        lowest_cost[1].p = -1;
    }else{
        lowest_cost[2].id_h = 2;
        lowest_cost[2].id_p = total_price;
        lowest_cost[2].p = -1;
    }

    pthread_exit(NULL);
}

void fill_array(study_t* array){
    for(int i=0;i<ARRAY_SIZE;i++){
        (array+i)->id_h = rand() % 3;
        (array+i)->id_p = rand() % 5;
        (array+i)->p = (rand() % 15) + 1;
    }
}

int main(){
    
    srand(time(NULL));

    pthread_t thread[THREADS];

    vec1_index = 0;
    vec2_index = 0;
    vec3_index = 0;
    filter = 0;
    compute = 0;

    fill_array(vec);
    
    for(int i=0;i<3;i++)
        pthread_mutex_init(&mutex[i], NULL);
        
    pthread_cond_init(&cond, NULL);

    int* partitions = (int*)calloc(THREADS, sizeof(int));

    for(int i=0;i<THREADS;i++){
        *(partitions+i) = (i+1);
        if(i < 3)
            pthread_create(&thread[i], NULL, filtering, (void*)(partitions+i));
        else
            pthread_create(&thread[i], NULL, computing, (void*)(partitions+i));
    }

    for(int i=0;i<THREADS;i++){
        pthread_join(thread[i], NULL);
    }
    
    printf("\tLowest prices of the set of 5 products each supermarket\n\n");
    for(int i=0;i<3;i++){
        printf("Supermarket: %d\nAverage price: %d\n\n", lowest_cost[i].id_h, lowest_cost[i].id_p);
    }
    

    for(int i=0;i<3;i++)
        pthread_mutex_destroy(&mutex[i]);

    pthread_cond_destroy(&cond);

    pthread_exit(NULL);
}