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

#define THREADS 5
#define ARRAY_SIZE 1000

pthread_cond_t cond;
pthread_mutex_t mutex;
int result[ARRAY_SIZE];
int data[ARRAY_SIZE];
int empty;

void* calculations(void *arg){
    
    int upper_bound = *((int*)arg);
    int index = upper_bound/200;

    for(int i=upper_bound-ARRAY_SIZE/THREADS;i<upper_bound;i++){
        result[i] = data[i] * 10 + 2;
    }
    
    // Todas as threads menos a primeira devem passar por este excerto de código.
    if(upper_bound > 200){
        pthread_mutex_lock(&mutex);
        
        // Caso o empty seja maior ou igual ao index da thread, é sinal que deve avançar.
        while(empty < index){
            // Vai "adormecer" uma thread e libertar o mutex especificado no segundo parâmetro.
            pthread_cond_wait(&cond, &mutex);
        }
        pthread_mutex_unlock(&mutex);
    }

    printf("Thread %d\n\n", upper_bound/200);
    for(int i=upper_bound-ARRAY_SIZE/THREADS;i<upper_bound;i++){
        printf("%d|", result[i]);
    }
    printf("\n\n");
    
    if(upper_bound < 1000){
        
        pthread_mutex_lock(&mutex);
        empty++;
        // É feito um broadcast para acordar todas as eventuais threads que estejam "adormecidas" em pthread_cond_wait().
        // Na ocasião de usar um signal, podia acordar outra thread qualquer que não a desejada (próxima).
        pthread_cond_broadcast(&cond);
        pthread_mutex_unlock(&mutex);
        
    }

    pthread_exit(NULL);
}

void fill_array(int* array){
    for(int i=0;i<ARRAY_SIZE;i++){
        *(array+i) = rand() % ARRAY_SIZE;
    }
}

int main(){
    
    srand(time(NULL));

    pthread_t thread[THREADS];

    fill_array(data);
    
    empty = 1;

    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond, NULL);

    int* partitions = (int*)calloc(5, sizeof(int));

    for(int i=0;i<THREADS;i++){
        *(partitions+i) = (i+1)*ARRAY_SIZE/THREADS;
        pthread_create(&thread[i], NULL, calculations, (void*)(partitions+i));
    }

    for(int i=0;i<THREADS;i++){
        pthread_join(thread[i], NULL);
    }

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);

    pthread_exit(NULL);

}