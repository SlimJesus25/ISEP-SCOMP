#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>

void handle_USR1(int signio, siginfo_t *sinfo, void *context){
    printf("%d [SIGUSR1] has been captured and was sent by the process with PID %d", sinfo->si_signo, sinfo->si_pid);
}

int main(){

    struct sigaction act;

    memset(&act, 0, sizeof(act));

    sigemptyset(&act.sa_mask); //Unblock all signals

    act.sa_handler = handle_USR1; //Define who the program should appeal when the signal (to refer in sigaction) is received.
    act.sa_flags = SA_SIGINFO;

    sigaction(SIGUSR1, &act, NULL);

    for(;;){
        printf("|\n");
        sleep(1);
    }
}