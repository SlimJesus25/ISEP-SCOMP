#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>


typedef struct{
	char string[3];
	int roundn;
} struct1;


int main(){

	int fd[2];
	int status;

	pid_t ppid = getpid();
	
	pipe(fd);

	for(int i=0;i<10;i++){
		
		if(fork()==0){
			break;
		}
	}

	if(ppid ==getpid()){
		close(fd[0]); 			//Close read
	}

	if(ppid != getpid()){
		close(fd[1]); 			//Close Write
	}

	for(int j=1;j<11;j++){    //ROUNDS
		
		if(ppid==getpid()){
			struct1 round;
			strcpy(round.string,"Win");
			round.roundn=j;
			write(fd[1],&round,sizeof(struct1));
			int spid = wait(&status);
			printf("Round %d won by PID: %d \n",WEXITSTATUS(status),spid);

		} else {
			struct1 ticket;
			read(fd[0],&ticket,sizeof(struct1));
			printf("\\o/ %s on round: %d  \\o/ \n",ticket.string,ticket.roundn);

			exit(ticket.roundn);
		}
	}

	return 0;
}