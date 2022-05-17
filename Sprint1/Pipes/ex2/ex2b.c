#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

typedef struct{
	char string[9];
	int n;
} struct1;

int main() {

struct1 in;
strcpy(in.string,"Tiagovski");
in.n=32;

struct1 out;

int fd[2];

pipe(fd);

pid_t pid = fork();

if(pid>0){
close(fd[0]);   //close read
write(fd[1],&in,sizeof(struct1));
close(fd[1]);

} else {
close(fd[1]);
read(fd[0],&out,sizeof(struct1));
close(fd[0]);

printf("Hey dad I readed %s \n",out.string);
printf("Hey dad I readed %d \n",out.n);


exit(1);
}



return 0;
}


