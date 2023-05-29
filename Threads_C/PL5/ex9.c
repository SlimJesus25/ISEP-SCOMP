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

#define TRAINS 5
#define ARRAY_SIZE 1000
#define CITY_A 0
#define CITY_B 1
#define CITY_C 2
#define CITY_D 3
#define CITY_A_TO_B_DURATION 5
#define CITY_B_TO_C_DURATION 3
#define CITY_B_TO_D_DURATION 2
#define CITY_A_TO_B 0
#define CITY_B_TO_C 1
#define CITY_B_TO_D 2
#define LINES 3

pthread_mutex_t mutex[LINES];

void* trip(void *arg){

    int train_number = *((int*)arg);

    int start_station = (rand() % 2 == 0) ? CITY_C : CITY_D;
    

    int actual_station = start_station;

    while(actual_station != CITY_A){
    
        switch(actual_station){
            case CITY_A:
                printf("Train %d arrived the final station (A)...\n", train_number);
                break;

            case CITY_B:
                pthread_mutex_lock(&mutex[CITY_A_TO_B]);
                printf("Train %d is now going from city B to city A\n", train_number);
                sleep(CITY_A_TO_B_DURATION);
                printf("Train %d arrived to city A\n\n", train_number);
                pthread_mutex_unlock(&mutex[CITY_A_TO_B]);
                actual_station = CITY_A;
                break;

            case CITY_C:
                pthread_mutex_lock(&mutex[CITY_B_TO_C]);
                printf("Train %d is now going from city C to city B\n", train_number);
                sleep(CITY_B_TO_C_DURATION);
                printf("Train %d arrived to city B\n\n", train_number);
                pthread_mutex_unlock(&mutex[CITY_B_TO_C]);
                actual_station = CITY_B;
                break;

            case CITY_D:
                pthread_mutex_lock(&mutex[CITY_B_TO_D]);
                printf("Train %d is now going from city D to city B\n", train_number);
                sleep(CITY_B_TO_D_DURATION);
                printf("Train %d arrived to city B\n\n", train_number);
                pthread_mutex_unlock(&mutex[CITY_B_TO_D]);
                actual_station = CITY_B;
                break;
        }
    }

    pthread_exit(NULL);
}

int main(){
    
    srand(time(NULL));

    for(int i=0;i<LINES;i++){
        pthread_mutex_init(&mutex[i], NULL);
    }

    for(int i=0;i<TRAINS;i++){
        pthread_cond_init(&cond[i], NULL);
    }

    pthread_t thread[TRAINS];

    int* train_number = (int*)calloc(TRAINS, sizeof(int));

    for(int i=0;i<TRAINS;i++){
        *(train_number+i) = i+1;
    }

    for(int i=0;i<TRAINS;i++){
        pthread_create(&thread[i], NULL, trip, (void*)(train_number+i));
    }

    for(int i=0;i<TRAINS;i++){
        pthread_join(thread[i], NULL);
    }
    
    printf("All trains arrived city A successfully!\n");

    for(int i=0;i<LINES;i++){
        pthread_mutex_destroy(&mutex[i]);
    }

    pthread_exit(NULL);

}