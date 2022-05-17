#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <time.h>

int USR1_COUNT=0;
void handle_signal();

int main() {

int pid = fork();
if(pid>0){
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
} else {
	struct timespec remaining, request = {0,10000000};
	pid_t parentid = getppid();
	for(int i=0; i<12;i++){
		kill(parentid, SIGUSR1);
		nanosleep(&request,&remaining);
	}
}
}

void handle_signal(){
USR1_COUNT++;
printf("SIGUSR1 signal captured, USR1_COUNTER = %d",USR1_COUNT);
}

