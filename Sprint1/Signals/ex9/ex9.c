#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>

volatile sig_atomic_t flag = 0;

void task_a(){
    printf("\nEntered Task A!\n");
    for(int i=0;i<3;i++){
        sleep(1);
        printf("%d seconds passed\n", i+1);
    }
}

void task_b(){
    printf("\nEntered Task B!\n");
    srand(time(NULL));
    int sec = rand() % 6;
    sleep(sec);
    printf("Sleeped %d seconds\n", sec);
}

void task_c(){
    printf("\nEntered Task C!\n");
    sleep(2);
}

void handler_usr1(int signo){
    flag = 1;
}

int main(){

    struct sigaction act;

    memset(&act, 0, sizeof(act));

    act.sa_handler = handler_usr1;

    sigaction(SIGUSR1, &act, NULL);

    pid_t p = fork();

    if(p > 0){
        task_a();
        kill(p, SIGUSR1);
    }else if(p == 0){
        task_b();
        while(flag == 0);
        task_c();
    }
    wait(NULL);

    return 0;
}