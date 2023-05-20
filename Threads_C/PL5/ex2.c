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

#define MAX_NAME_CHARS 20
#define ARRAY_SIZE 5

typedef struct{
    int number;
    char name[MAX_NAME_CHARS];
    float grade;
}aluno_t;

void* thread_func(void *arg){
    aluno_t* aluno = (aluno_t*)arg;
    
    printf("Informação do estudante\nNúmero: %d\nNome: %s\nNota: %f\n\n\n"
    , aluno->number, aluno->name, aluno->grade);
    pthread_exit((void*)NULL);
}



int main(){
    
    aluno_t* array = (aluno_t*)calloc(ARRAY_SIZE, sizeof(aluno_t));
    int args[5];
    pthread_t threads[5];

    for(int i=0;i<5;i++){
        snprintf((array+i)->name, MAX_NAME_CHARS*sizeof(char), "thread%d", i);
        (array+i)->number = i*1000;
        (array+i)->grade = (float)i*0.23;
    }
    
    for(int i=0;i<5;i++){
        
        // Como as threads partilham as mesmas variáveis da heap, ao enviar o endereço para a iteração que queremos, é possível que acedam a essa informação.
        pthread_create(&threads[i], NULL, thread_func, (void*)(array+i));
    }

    for(int i=0;i<5;i++){
        pthread_join(threads[i], NULL);
    }

    pthread_exit((void*)NULL);

}