#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/syscall.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

void handle_ALRM(int signo, siginfo_t *sinfo, void *context){
    char buffer[50];

    snprintf(buffer, 75, "\n\nYou were too slow and this program will end!\n");
    
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

    char* input_string = (char*)calloc(100, sizeof(char));
    printf("\nEnter text: ");
    alarm(10);
    scanf(" %s", input_string);
    printf("\n\nSize of the %s: %ld\n", input_string, strlen(input_string));
    exit(EXIT_SUCCESS);
}