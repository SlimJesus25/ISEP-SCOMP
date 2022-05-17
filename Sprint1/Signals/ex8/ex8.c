#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>

volatile sig_atomic_t counter = 5;

void handle_child(int signo) {
	counter--;
	printf("%d child proccess executing \n", counter);
}

int main()
{
	struct sigaction act;
	memset(&act, 0, sizeof(struct sigaction));
	act.sa_flags = SA_NOCLDWAIT;
	act.sa_handler = handle_child;
	sigaction(SIGCHLD, &act, NULL);
	
	
	pid_t pid;
	
	for(int i=0;i<5;i++){

		pid=fork();
		
		if(pid==0){
			for(int j=i*200;j<=((i+5)*200);j++){
				printf("Result: %d\n", j);
				sleep(1);
			}
			kill(getppid(),SIGCHLD);
			exit(1);
			
		}else{
			pause();
			if(counter==0){
				printf("Cya.");
			}
		}
		}

		for(int k=0;k<5;k++){
			wait(NULL);
		}
	}
	



