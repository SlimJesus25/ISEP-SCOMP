#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#define BUFFER_SIZE 80
int vec1[1000],vec2[1000],somas[5];

void fill_array(){
	int i;
	for(i=0;i<1000;i++){
			vec1[i]=i+1;
			vec2[i]=i+1;
	}

}

int main(void){
	pid_t a;
	int fd[2];
	fill_array();
	int nTotal,i,status,y;	
	nTotal=0;
	if(pipe(fd)==-1){
		perror("Pipe está a dar erro, tente novamente mais tarde.\n");
		return EXIT_FAILURE;
	}
	for(i=0;i<5;i++){
	a=fork();
	if(a<0){
		perror("A fork não funcionou.\n");
		return EXIT_FAILURE;
	}
	if(a>0){
		read(fd[0],&somas[i],sizeof(int));
		wait(&status);
		nTotal+=somas[i];
	} else {
		close(fd[0]);
		int n1=0;
		for(y=i*200;y<(i+1)*200;y++){
				n1+=vec1[y];
				n1+=vec2[y];
		}
		write(fd[1],&n1,sizeof(int));
		close(fd[1]);
		exit(EXIT_SUCCESS);
	}
	
	}
	printf("Total soma=%d.\n",nTotal);	
	
	
	return EXIT_SUCCESS;
}
