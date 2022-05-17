#include <stdio.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <time.h>

void simulate2(){

}

void handle_child(int signo) {
	simulate2();
}

int simulate1(){
	int n;
	
	n=1 + (rand() % 5); 

	printf("Number: %d\n", num);

	if(num%2==0){
		kill(getppid(), SIGUSR1);
		return 1;
	}else{
		kill(getppid(), SIGUSR2);
		return 0;
	}
	wait(0);
}

int main(void){
	int count=0;

	struct sigaction act;
	memset(&act, 0, sizeof(struct sigaction)); // limpar a estrutura
	act.sa_handler = handle_child;
	sigaction(SIGCHLD, &act, NULL);
	pid_t a;
	
	
	for(int i=0;i<50;i++){

		a=fork();

		if(a==0){
			
		}else{
			while(i<24){
				pause();
			}
			
		}
	}

	printf("Inefficient algorithm!\n");
				kill(a, SIGKILL);

	return 0;
}
	
	
	
	
	
