#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/syscall.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

volatile sig_atomic_t gate;

void handle_USR1(int signo, siginfo_t *sinfo, void *context){
    gate++;
}

void task_A(int time_to_sleep){
    sleep(time_to_sleep);
    printf("Task A done!\n");
}

void task_B(int time_to_sleep){
    sleep(time_to_sleep);
    printf("Task B done!\n");
}

void task_C(int time_to_sleep){
    sleep(time_to_sleep);
    printf("Task C done!\n");
}

int main(){

    gate = 0;

    struct sigaction act;
    
    time_t t;

    srand((unsigned) time(&t));

    memset(&act, 0, sizeof(act));
    act.sa_sigaction = handle_USR1;
    sigfillset(&act.sa_mask);
    act.sa_flags = SA_NOCLDWAIT;

    sigaction(SIGUSR1, &act, NULL);

    pid_t p;

    p = fork();

    if(p < 0){
        exit(EXIT_FAILURE);
    }

    if(p == 0){
        int r = rand() % 5;
        task_B(r);
        while(gate == 0);
        task_C(1);
        exit(EXIT_SUCCESS);
    }

    task_A(3);
    kill(p, SIGUSR1);
    wait(NULL);
    printf("Job is complete!\n");
}