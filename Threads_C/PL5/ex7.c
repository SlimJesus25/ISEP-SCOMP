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

#define THREADS 16
#define UPPER_BOUND 49
#define LOWER_BOUND 1
#define KEYS 8000
#define PLAY_SLIP_TRIES 5

typedef struct{
    int* play_slip;
    int bound;
}play_t;

int matrix[PLAY_SLIP_TRIES][2];
pthread_mutex_t mutex[PLAY_SLIP_TRIES];
int keys[KEYS][PLAY_SLIP_TRIES];

void* partial_accounting(void *arg){
    
    play_t* bound = (play_t*)arg;
    
    for(int i=bound->bound;i<bound->bound+(KEYS/THREADS);i++){
        for(int k=0;k<PLAY_SLIP_TRIES;k++){
            for(int j=0;j<PLAY_SLIP_TRIES;j++){
                if(*(bound->play_slip+j) == keys[i][k]){
                    pthread_mutex_lock(&mutex[j]);
                    matrix[j][1]++;
                    pthread_mutex_unlock(&mutex[j]);
                }
            }
        }
    }

    pthread_exit(NULL);
}

int random_number(){
    return rand() % (UPPER_BOUND - LOWER_BOUND + 1) + LOWER_BOUND;
}

void choose_five_number(int* play_slip){
    for(int i=0;i<5;i++){
        *(play_slip+i) = random_number();
    }
}

void fill_database(){
    for(int i=0;i<KEYS;i++){
        for(int j=0;j<5;j++){
            keys[i][j] = random_number();
        }
    }
}

int main(){
    
    srand(time(NULL));

    pthread_t thread[THREADS];

    play_t* play = (play_t*)calloc(THREADS, sizeof(play_t));

    int* play_slip = (int*)calloc(5, sizeof(int));

    fill_database();

    choose_five_number(play_slip);
    
    printf("Play Slip\n");
    for(int i=0;i<PLAY_SLIP_TRIES;i++){
        matrix[i][0] = *(play_slip+i);
        matrix[i][1] = 0;
        printf("|%d|", *(play_slip+i));
    }
    printf("\n\n");

    for(int i=0;i<THREADS;i++){
        (play+i)->play_slip = play_slip;
        (play+i)->bound = i*500;
    }

    for(int i=0;i<PLAY_SLIP_TRIES;i++){
        pthread_mutex_init(&mutex[i], NULL);
    }

    for(int i=0;i<THREADS;i++){
        pthread_create(&thread[i], NULL, partial_accounting, (void*)(play+i));
    }

    for(int i=0;i<THREADS;i++){
        pthread_join(thread[i], NULL);
    }
    
    free(play);
    free(play_slip);

    for(int i=0;i<PLAY_SLIP_TRIES;i++){
        pthread_mutex_destroy(&mutex[i]);
    }
    
    printf("Statistics\n");
    for(int i=0;i<PLAY_SLIP_TRIES;i++){
        printf("%d found %d\n", matrix[i][0], matrix[i][1]);
    }

    pthread_exit((void*)NULL);

}