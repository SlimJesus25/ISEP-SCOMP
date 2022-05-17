#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>

#define SIZE_ARRAY 5

int i;

typedef struct{
    char cmd[32];
    int tempo;
} comando;

comando *ptr;

void handler_child(){
    char txt[100];
	write(STDOUT_FILENO,"Execution ended!", 17);
}

void handler_alarm(){
    printf("PID: %d", getpid());
    kill(getpid(), SIGKILL);
    char txt[100];
    snprintf(txt, 100, "The command %s didn’t end in its allowed time!", ptr->cmd);
    write(STDOUT_FILENO, txt, strlen(txt));
}

int main(){

    struct sigaction act, act2;
    memset(&act, 0, sizeof(act));

    act.sa_handler = handler_child;
    act2.sa_handler = handler_alarm;

    sigaction(SIGCHLD, &act, NULL);
    sigaction(SIGALRM, &act2, NULL);

    comando arr[] = {{"./a", 10}, {"./b", 5}, {"./c", 8}, {"./d", 4}};

    pid_t p = fork();
    if(p < 0){
        perror("Error creating process!");
        return 1;
    }
    if(p == 0){
        for(i=0;i<sizeof(arr)/sizeof(comando);i++){
            ptr = &arr[i];
            printf("%d seconds to execute %s", arr[i].tempo, arr[i].cmd);
            execlp(arr[i].cmd, arr[i].cmd, (char*)NULL);
            exit(0);
        }
    }else{
        alarm(arr[i].tempo);
        wait(NULL);
    }

    return 0;
}