#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#define SIGNAL SIGUSR1

void handle_USR1(int signo, siginfo_t *sinfo, void *context){
    char buffer[200];
    pid_t p = sinfo->si_pid;
    snprintf(buffer, 100, "Captured a SIGUSR1 sent by the process with PID %d\n", p);
   
    int buffer_length = strlen(buffer);
    write(STDOUT_FILENO, buffer, buffer_length);
}

int main(){

    struct sigaction act;

    memset(&act, 0, sizeof(struct sigaction));
    sigemptyset(&act.sa_mask);
    act.sa_sigaction = handle_USR1;
    act.sa_flags = SA_SIGINFO;

    sigaction(SIGUSR1, &act, NULL);

    sleep(15);

    return 0;
}