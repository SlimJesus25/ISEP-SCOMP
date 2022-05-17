#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>

#define MAX_SIZE 256

void server_converting_characters(char *str) {

	for(int i = 0; i < strlen(str); i++) {
		if(str[i] > 96 && str[i] < 123)
			*(str+i) -= 32;
		else if(str[i] > 64 && str[i] < 91)
		    *(str+i) += 32;
    }
}

int main(){

    char txt[MAX_SIZE];

    int fd1[2], fd2[2];

    if(pipe(fd1) == -1){
        perror("Up pipe failed!");
        return 1;
    }
    if(pipe(fd2) == -1){
        perror("Down pipe failed!");
        return 1;
    }

    pid_t p = fork();

    if(p < 0){
        perror("Error creating child process!");
        return -1;
    }
    if(p > 0){ // Server Side

        read(fd1[0], txt, MAX_SIZE);
        //printf("\nValue received by txt: %s", txt);
        close(fd1[0]);
        // Server side is waiting for the client to send some text.
        // strcpy(write_msg, server_converting_characters(read_msg));

        char* msg = txt;
        server_converting_characters(msg);

        //strcpy(txt, server_converting_characters(msg));
        //printf("\nValue that txt contains: %s", msg);
        write(fd2[1], msg, MAX_SIZE * sizeof(char));
        close(fd2[1]);

    }else{ // Client Side

        char msg[10] = "sCoMp";

        printf("\n\nOriginal word: %s\n", msg);
        write(fd1[1], msg, MAX_SIZE * sizeof(char));
        close(fd1[1]);

        read(fd2[0], txt, MAX_SIZE * sizeof(char));
        close(fd2[0]);

        printf("\n\nNew word: %s\n", txt);

    }

    return 0;

}