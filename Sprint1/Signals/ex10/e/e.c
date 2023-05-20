#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <time.h>

#define SIGNAL SIGUSR1

volatile sig_atomic_t USER1_counter;

void handle_USR1(int signo, siginfo_t *sinfo, void *context){
    sleep(2);
    char buffer[50];
    
    USER1_counter++;

    snprintf(buffer, 75, "SIGUSR1 signal captured: USR1_counter = %d”\n", USER1_counter);
   
    int buffer_length = strlen(buffer);
    write(STDOUT_FILENO, buffer, buffer_length);
}


// Era suposto o handler estar a ser constantemente interrompido, mas o SIGINT é imediatamente enviado.
int main(){

    struct sigaction act;

    memset(&act, 0, sizeof(act));
    sigemptyset(&act.sa_mask);
    act.sa_sigaction = handle_USR1;
    act.sa_flags = SA_SIGINFO;

    sigaction(SIGNAL, &act, NULL);

    pid_t p = fork();

    if(p < 0){
        printf("An error occurred on fork!\n");
        exit(EXIT_FAILURE);
    }

    int counter = 0;

    // 10ms = 10 000 000ns
    struct timespec request = {0, 10000000}; 

    for(;;){
        if(p == 0){
            if(counter == 12){
                sleep(1);
                printf("Sending SIGINT to %d...\n", getppid());
                kill(getppid(), SIGINT);
                break;
            }

            kill(getppid(), SIGUSR1);
            nanosleep(&request, NULL);
            counter++;
        }
        if(p > 0){
            printf("Im working\n");
            sleep(1);
        }
    }

    printf("I'm the child and I was adopted by init process (Original PPID: %d)", getppid());
    exit(EXIT_SUCCESS);

    return 0;
}