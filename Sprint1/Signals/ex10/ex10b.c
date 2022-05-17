#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

void handle_SIGALRM(int signo){
	char message[]="To slow, that is why the program will end!\n";
	int nbytes=strlen(message);
	write(STDOUT_FILENO,message, nbytes);
	raise(SIGKILL);
}

int main(void){
	int i;
	pid_t a;
	int count=0;
	char input[100];

	struct sigaction act;
	memset(&act, 0,sizeof(struct sigaction));
	sigemptyset(&act.sa_mask);
	act.sa_handler = handle_SIGALRM;
	sigaction(SIGCHLD,&act,NULL);
	a=fork();
	if(a>0){

	printf("Write something fast:");

	while(fgets(input,sizeof(input),stdin)==NULL){
			
	}
	
	signal(SIGCHLD,SIG_IGN);
	
	} else{
		sleep(10);
		exit(EXIT_SUCCESS);
	}
	
	for(i=0;i<strlen(input);i++){
		if(input[i]!=' '){
			if(input[i]!='\n'){
			count++;
		}
		}
	}
	printf("Your word have %d characters. Processing your word\n",count);

	sleep(20);
	
	printf("Done!\n");

	return EXIT_SUCCESS;
}