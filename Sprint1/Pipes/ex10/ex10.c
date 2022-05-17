#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>

int main() {
	int fd[2]; // Dad read
	int fdd[2];   // dad writes
	int lower =1 , upper=5;

	pipe(fd);
	pipe(fdd);


	if(fork() ==0){
		int flag;
		int bet = (rand() % (upper-lower+1)) + lower;
		close(fd[0]);
		close(fdd[1]);
		read(fdd[0],&flag,sizeof(int));
		if(flag==1){
			printf("I bet on %d \n",bet);
			write(fd[1],&bet,sizeof(int));
			
		}
		exit(1);

		

	} else {
		int credit=20;
		int number = (rand() % (upper-lower+1)) + lower;
		int numbet;
		int one=1;
		close(fd[1]);
		close(fdd[0]);
		write(fdd[1],&one,sizeof(int));
		
		read(fd[0],&numbet,sizeof(int));
		if(numbet==number){
			credit+=10;
			printf("Congrats you Won! \n");
		} else {
			credit-=5;
			printf("Sorry you lost :( \n");

		}

		printf("Current credits: %d \n",credit);
	}
}


