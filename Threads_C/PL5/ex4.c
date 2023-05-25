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

#define MATRIX_COLUMNS 8
#define MATRIX_ROWS 8
#define FILL_THREADS 2
#define CALC_THREADS 8
#define THREADS FILL_THREADS + CALC_THREADS

typedef struct{
    int* matrix1;
    int* matrix2;
    int* matrix3;
    int row;
}info_t;

void* fill_matrix(void *arg){
    
    int *matrix = (int*)arg;

    for(int i=0;i<MATRIX_ROWS;i++){
        for(int j=0;j<MATRIX_COLUMNS;j++){
            *(matrix+i*MATRIX_ROWS+j) = rand() % 599;
        }
    }
    
    pthread_exit((void*)NULL);
}

void* multiply_matrix(void *arg){

    info_t *info = (info_t*)arg;
    int row = info->row;

    for(int j=0;j<MATRIX_ROWS;j++){
        *(info->matrix3+row*MATRIX_ROWS+j) = *(info->matrix1+row*MATRIX_ROWS+j) * *(info->matrix2+row*MATRIX_ROWS+j);
    }
    pthread_exit((void*)NULL);
}

void print_matrix(int* matrix){
    for(int i=0;i<MATRIX_ROWS;i++){
        printf("\n");
        for(int j=0;j<MATRIX_COLUMNS;j++){
            printf("%d|", *(matrix+i*MATRIX_ROWS+j));
        }
    }
}

int main(){

    pthread_t threads[THREADS];

    int* matrix1 = (int*)calloc(MATRIX_COLUMNS*MATRIX_ROWS, sizeof(int));
    int* matrix2 = (int*)calloc(MATRIX_COLUMNS*MATRIX_ROWS, sizeof(int));
    int* matrix3 = (int*)calloc(MATRIX_COLUMNS*MATRIX_ROWS, sizeof(int));

    info_t* info = (info_t*)calloc(CALC_THREADS, sizeof(info_t));
    
    // 2 threads are created to fill both matrixes.
    for(int i=0;i<FILL_THREADS;i++){
        int* m;
        if(i == 0)
            m = matrix1;
        else
            m = matrix2;

        pthread_create(&threads[i], NULL, fill_matrix, (void*)m);
    }

    // The main thread must wait for the 2 threads previously created.
    for(int i=0;i<FILL_THREADS;i++){
        pthread_join(threads[i], NULL);
    }

    printf("Matrix 1\n");
    print_matrix(matrix1);
    printf("\nMatrix 2\n");
    print_matrix(matrix2);
    printf("\n");

    for(int i=0;i<CALC_THREADS;i++){
        (info+i)->row = i;
        (info+i)->matrix1 = matrix1;
        (info+i)->matrix2 = matrix2;
        (info+i)->matrix3 = matrix3;
        pthread_create(&threads[i], NULL, multiply_matrix, (void*)(info+i));
    }

    // Main thread waits for the final matrix.
    for(int i=0;i<CALC_THREADS;i++){
        pthread_join(threads[i], NULL);
    }

    printf("Multiplied matrix");
    print_matrix((info+2)->matrix3);
    
    pthread_exit((void*)NULL);

}