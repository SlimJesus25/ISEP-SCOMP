#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/syscall.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#define CHILDREN 5

volatile sig_atomic_t pid_array[CHILDREN];

typedef struct{
    char cmd[128]; // Command name.
    int time_cap; // Time limit to complete (in seconds).
} command_t;

void handle_ALRM(int signo, siginfo_t *sinfo, void *context){
    char buffer[50];

    snprintf(buffer, 75, "\n\n\tA execução deste programa mais lenta que o esperado!\n");

    for(int i=0;i<CHILDREN;i++){
        kill(pid_array[i], SIGINT);
    }
    
    int buffer_length = strlen(buffer);
    write(STDOUT_FILENO, buffer, buffer_length);
    exit(EXIT_FAILURE);
}

int main(){

    struct sigaction act;

    memset(&act, 0, sizeof(act));
    act.sa_sigaction = handle_ALRM;
    sigfillset(&act.sa_mask);

    sigaction(SIGALRM, &act, NULL);

    command_t array[10];

    for(int i=0;i<10;i++){
        strcpy(array[i].cmd, "/home/ricardo/Documents/SCOMP/scomp2023_2de_1210828/Sprint1/Signals/ex15/teste"); // Criar um programa simples (como diz nas sugestões do exercício).
        array[i].time_cap = 14-i;
    }


    for(int i=0;i<CHILDREN;i++){
        pid_array[i] = fork();
        if(pid_array[i] == 0){
            char arg[15];
            snprintf(arg, 15*sizeof(char), "Teste %d", i);
            //printf("\nExecuting %s and sending %s as arg\n", array[i].cmd, arg);
            int r = execlp(array[i].cmd, arg);
            if(r == -1){
                perror("Error execlp!");
                exit(EXIT_FAILURE);
            }
        }
        alarm(array[i].time_cap);
        wait(NULL);
        alarm(0); // Esta instrução cancela qualquer alarme preparado anteriormente.
    }

    
}