#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>


int main() {



int fd[2];

pipe(fd);

FILE *fp;
char *text;
long numBytes;


fp = fopen("file.txt", "r");


fseek(fp,0L,SEEK_END);
numBytes = ftell(fp);
fseek(fp,0L,SEEK_SET);

text= (char*) calloc(numBytes,sizeof(char));

fread(text,sizeof(char),numBytes,fp);
fclose(fp);

pid_t pid = fork();


if(pid>0){

close(fd[0]);   //close read

write(fd[1],text,strlen(text)+1);
close(fd[1]);

} else {
char * readed_text = (char*) calloc(numBytes,sizeof(char));

close(fd[1]);
read(fd[0],readed_text,strlen(text)+1);

printf("Papai vou fazer um bolo preciso de: %s \n",readed_text);
exit(1);
}



return 0;
}


