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

/**
 * 1.
 * a) São criados 5 processos onde o pai cria 2 processos, o primeiro filho cria 2 processos e o primeiro filho do primeiro filho criado pelo pai cria 1 processo.
 * b) São criadas 2 threads, pois o bloco de código onde são criadas threads, vai ser percorrido apenas por 2 processos.
 */

int main(){
    // 1º processo criado.
    pid_t p = fork();

    if(p == 0){
        // 2º processo criado.
        fork();
        phtread_t thread_id;
        // Cada um dos 2 processos que vão executar este bloco de código criam 1 thread, ou seja, 2 threads são criadas.
        pthread_create(&thread_id, NULL, thread_func, NULL);
        pthread_join(thread_id, NULL);
    }
    // 3º, 4º e 5º processos são criados.
    fork();
    
    // ...
}