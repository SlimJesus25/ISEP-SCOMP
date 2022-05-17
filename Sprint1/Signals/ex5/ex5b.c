#include <sys/wait.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

void handle_usr1(int signio){

    char str[] = "I won't let the process end with CTRL-C";

    write(STDOUT_FILENO, str, sizeof(str));
}

int main(){

    struct sigaction act;

    memset(&act, 0, sizeof(act));

    sigemptyset(&act.sa_mask);
    act.sa_handler = handle_usr1;
    sigaction(SIGINT, &act, NULL);

    for(;;){
        printf("I Like Signal\n");
        sleep(1);
    }
}