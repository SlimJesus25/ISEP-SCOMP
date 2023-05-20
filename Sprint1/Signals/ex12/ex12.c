#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/syscall.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#define SIGNAL SIGUSR1
#define CHILDS 5

volatile sig_atomic_t child_counter_usr1;
volatile sig_atomic_t child_counter_chld;


void handle_USR1(int signo, siginfo_t *sinfo, void *context){
    child_counter_usr1--;
}

void handle_CHLD(int signo, siginfo_t *sinfo, void *context){
    child_counter_chld--;
}

void print_sequence(int min_lim, int max_lim){
    for(int i=min_lim;i<max_lim;i++){
        //printf("%d|", i);
    }
    
    printf("From %d to %d -> ", min_lim, max_lim-1);
}

int main(){

    child_counter_chld = CHILDS;
    child_counter_usr1 = CHILDS;

    struct sigaction act;

    memset(&act, 0, sizeof(act));
    act.sa_sigaction = handle_USR1;
    //sigfillset(&act.sa_mask);
    // Faz com que, durante a execução do handler, se for recebido um sinal do mesmo tipo, não seja ignorado. Obs: Se um sigfillset() for usado, esta flag perde o efeito.
    act.sa_flags = SA_NODEFER;
    
    sigaction(SIGNAL, &act, NULL);

    struct sigaction act_chld;

    memset(&act_chld, 0, sizeof(act_chld));
    act_chld.sa_sigaction = handle_CHLD;
    act_chld.sa_flags = SA_NOCLDWAIT | SA_NOCLDSTOP;

    sigaction(SIGCHLD, &act_chld, NULL);

    pid_t p;

    int min=0;
    int max=0;

    for(int i=0;i<CHILDS;i++){
        min = i*200;
        max = (i+5)*200;
        p = fork();

        if(p == 0){
            print_sequence(min, max);
            printf("Child %d sending SIGUSR1 to %d (PPID)\n", i, getppid());
            kill(getppid(), SIGUSR1);
            // Sempre que um filho morre, é enviado um SIGCHLD para o processo pai (que por defeito é ignorado).
            exit(EXIT_SUCCESS);
        }
    }

    while(child_counter_usr1 > 0 && child_counter_chld > 0){
        pause(); // Antes do pai chegar aqui, os filhos podem já ter enviado o 
        //SIGUSR1 que vai resultar num pause infinito à espera de algo que não vai chegar
        printf("CHILD_COUNTER USR1: %d\n", child_counter_usr1);
        printf("CHILD_COUNTER CHLD: %d\n\n", child_counter_chld);
    }
    
    // Para solucionar o problema acima, utilizou-se a flag SA_NODEFER que faz com que os sinais do próprio tipo, durante a execução do handler, não sejam ignorados.
    // Ou seja, quando o handler é interrompido, vai tratar do sinal que o interrompeu, e após isso volta a tratar exatamente no ponto onde estava da função que foi interrompida.
    // Foi também implementada uma função que vai tratar dos sinais do tipo SIGCHLD enviados pela função exit() que todos os filhos fazem.

    for(int i=0;i<CHILDS;i++){
        wait(NULL);
    }

    exit(EXIT_SUCCESS);
}