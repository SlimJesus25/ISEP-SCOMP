#include <sys/wait.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

void handle_Usr1(int signio){

    char str[] = "I won't let the process end with CTRL-\\";

    write(STDOUT_FILENO, str, sizeof(str));
}

int main(){

    struct sigaction act;

    memset(&act, 0, sizeof(act));

    sigemptyset(&act.sa_mask);
    act.sa_handler = handle_Usr1;
    sigaction(SIGQUIT, &act, NULL);

    for(;;){
        printf("I Like Signal\n");
        sleep(1);
    }
}