#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#define SIGNAL SIGINT

void handle_USR1(int signo, siginfo_t *sinfo, void *context){
    char buffer[50];
    snprintf(buffer, 50, "I won’t let the process end with CTRL-C!”\n");
   
    int buffer_length = strlen(buffer);
    write(STDOUT_FILENO, buffer, buffer_length);
}

int main(){

    struct sigaction act;

    memset(&act, 0, sizeof(struct sigaction));
    sigemptyset(&act.sa_mask);
    act.sa_sigaction = handle_USR1;
    act.sa_flags = SA_SIGINFO;

    sigaction(SIGNAL, &act, NULL);

    for(;;){
        printf("I like signals\n");
        sleep(1);
    }

    return 0;
}