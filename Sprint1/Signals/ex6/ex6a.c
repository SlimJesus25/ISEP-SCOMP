#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

int USR1_COUNT=0;
void handle_signal();

int main() {

struct sigaction act;

memset(&act,0,sizeof(struct sigaction));
sigemptyset(&act.sa_mask);
act.sa_sigaction = handle_signal;
act.sa_flags = SA_SIGINFO;
sigaction(SIGUSR1,&act, NULL);

for(;;){
printf("Loop\n");
sleep(1);
}

}

void handle_signal(){
USR1_COUNT++;
printf("SIGUSR1 signal captured, USR1_COUNTER = %d",USR1_COUNT);
}

