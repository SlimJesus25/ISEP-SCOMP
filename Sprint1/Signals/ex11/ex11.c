#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/syscall.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#define SIGNAL SIGUSR1

void handle_USR1(int signo, siginfo_t *sinfo, void *context){
    char buffer[50];

    snprintf(buffer, 75, "SIGUSR1 signal captured”\n");
   
    int buffer_length = strlen(buffer);
    write(STDOUT_FILENO, buffer, buffer_length);
}

void blocked_signals(const sigset_t *set){
    sigfillset(set);
    sigprocmask(SIG_BLOCK, NULL, NULL);
    printf("\tWhen SIGUSR1 is received, these signals are blocked\n\n");
    for(int i=1;i<22;i++){
        int blocked = sigismember(set, i);
        if(blocked == 1){
            printf("Signal %d is blocked\n", i);
        }else{
            printf("Signal %d is NOT blocked\n", i);
        }
    }
    sigprocmask(SIG_UNBLOCK, NULL, NULL);

    printf("\nNote: There are 2 exceptions (SIGKILL and SIGSTOP) that are unstoppable!");
}

// Neste exercício foram usadas as seguintes funções:
// - sigfillset para atrasar todos os sinais, que forem "atrasáveis", durante a execução do handler;
// - sigismember para verificar se, consoante uma mascara passada por parâmetro, um sinal está definido para atrasar ou não;
// - sigprocmask para examinar, mudar ou ambos o sinal da máscara do processo que invoca esta função.

int main(){

    struct sigaction act;

    memset(&act, 0, sizeof(act));
    act.sa_sigaction = handle_USR1;
    //sigfillset(&act.sa_mask);
    //sigprocmask(SIG_BLOCK, &act.sa_mask, NULL); -> Bloqueia todos os sinais (bloqueáveis) em todo o scope do programa até ser chamada novamente com o 1º parâmetro a SIG_UNBLOCK.

    sigaction(SIGNAL, &act, NULL);

    blocked_signals(&act.sa_mask);

    for(;;){
        printf("Im working\n");
        sleep(1);
    }

    return 0;
}