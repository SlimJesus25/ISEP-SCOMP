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

#define ARRAY_SIZE 1000
#define THREADS 10

int n = 412164;

typedef struct{
    int* array;
    int min;
    int max;
}info_t;

void* thread_func(void *arg){
    
    // Conversão de void/bytes para info_t*.
    info_t* element = (info_t*)arg;
    
    // Reserva de espaço para 1 inteiro. NOTA: É necessário fazer esta reserva para depois retornar valores, pois
    // quando a thread morre, variáveis locais vão perder a alocação em memória, por isso, é necessário reservar
    // heap. Outra solução poderia ser utilizar variáveis globais.
    int* index = calloc(1, sizeof(int));

    for(int i=element->min;i<element->max;i++){
        if(*(element->array+i) == n){
            *(index) = i;
            pthread_exit((void*)index);
        }
    }
    *(index) = -1;
    pthread_exit((void*)index);
}

int main(){
    
    // Criação das threads.
    pthread_t threads[THREADS];
    
    // Criação de um array de 10 posições para enviar cada uma delas para as threads posteriormente criadas.
    info_t* element = calloc(THREADS, sizeof(info_t));
    
    // Apontador que vai conter os valores "random".
    int* tst_arr = (int*)calloc(ARRAY_SIZE, sizeof(int));
    
    // Atribuição de valores.
    for(int i=0;i<ARRAY_SIZE;i++){
        *(tst_arr+i) = pow(i, 2);
    }
    
    // Atribuição de valores ao minimo/máximo + colocação do apontador de todas as posições do apontador de info_t para o array previamente carregado (tst_arr).
    for(int i=0;i<THREADS;i++){
        (element+i)->array = tst_arr;
        (element+i)->min = i*ARRAY_SIZE/THREADS;
        (element+i)->max = (i+1)*ARRAY_SIZE/THREADS;
    }
    
    // Criação das threads enviado por argumento cada uma das posições do vetor.
    for(int i=0;i<THREADS;i++){
        pthread_create(&threads[i], NULL, thread_func, (void*)(element+i));
    }
    
    // Variável que armazena o resultado retornado pelas threads que deram exit.
    void* res;

    for(int i=0;i<THREADS;i++){
        pthread_join(threads[i], &res);
        int value = *((int*)res);

        if(value > 0){
            printf("Thread %d found %d in index %d\n", i, n, value);
        }
    }

    pthread_exit((void*)NULL);

}