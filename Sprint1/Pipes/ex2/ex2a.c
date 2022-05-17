#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

int main() {

char string[] = "Tiagovski";
char r_string[9];
int n = 32;
int r_n;

int fd[2];

pipe(fd);

pid_t pid = fork();

if(pid>0){
close(fd[0]);   //close read

write(fd[1],string,strlen(string)+1);
write(fd[1],&n,sizeof(n));
close(fd[1]);

} else {
close(fd[1]);
read(fd[0],r_string,strlen(string)+1);
read(fd[0],&r_n,sizeof(n));

printf("Hey dad I readed %s \n",r_string);
printf("Hey dad I readed %d \n",r_n);
exit(1);
}



return 0;
}


