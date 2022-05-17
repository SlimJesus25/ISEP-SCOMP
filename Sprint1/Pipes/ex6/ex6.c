#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {

int fd[2];
int vec1[1000];
int vec2[1000];
int sum=0, tmp=0;

for(int i=0;i<1000;i++){
	vec1[i]=1;
	vec2[i]=1;
}


pipe(fd);


for(int i=0;i<5;i++){
	pid_t pid=fork();


	if(pid>0){
		close(fd[1]); //close write
		read(fd[0],&tmp,sizeof(int));
		wait(NULL);
		sum +=tmp;
	} else {
		close(fd[0]); //close read
		tmp=0;
		for(int j=i*200;j<(i+1)*200;j++){
			tmp+=vec1[j];
			tmp+=vec2[j];
		}
		write(fd[1],&tmp,sizeof(int));
		close(fd[1]);
		exit(EXIT_SUCCESS);
	}
}

printf("Soma Total= %d \n",sum);


}


