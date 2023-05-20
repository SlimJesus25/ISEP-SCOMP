#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#define SIGNAL SIGUSR1

volatile sig_atomic_t USER1_counter;

void handle_USR1(int signo, siginfo_t *sinfo, void *context){
    char buffer[50];
    
    USER1_counter++;

    snprintf(buffer, 75, "SIGUSR1 signal captured: USR1_counter = %d”\n", USER1_counter);
   
    int buffer_length = strlen(buffer);
    write(STDOUT_FILENO, buffer, buffer_length);
}

int main(){

    struct sigaction act;

    memset(&act, 0, sizeof(act));
    sigemptyset(&act.sa_mask);
    act.sa_sigaction = handle_USR1;
    act.sa_flags = SA_SIGINFO;

    sigaction(SIGNAL, &act, NULL);

    for(;;){
        printf("Im working\n");
        sleep(1);
    }

    return 0;
}