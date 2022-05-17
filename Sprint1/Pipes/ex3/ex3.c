#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>
/*
Write a program that creates a child process and establishes with it a communication channel through a pipe. The
parent process should send two messages, “Hello World” and “Goodbye!”, and then closes the communication
channel. The child process should wait for data and print it as soon as it is available, terminating after all data is
received. The parent process must wait for its child to end, printing its PID and exit value.
*/

int main(){

    char read_msg[2][12];
    char write_msg[][15] = {"Hello World", "Goodbye!"};
    int fd[2];

    int status;

    if(pipe(fd) == -1){
        perror("Pipe failed!");
        return 1;
    }

    pid_t p = fork();

    if(p > 0){
        close(fd[0]);
        write(fd[1], write_msg, sizeof(write_msg));
        close(fd[1]);
    }else{
        close(fd[1]);
        read(fd[0], read_msg, sizeof(write_msg)); // read() is a blocking function, therefore it will wait for something in the pipe by default.
        close(fd[0]);
        printf("[CHILD] Received Message on pipe: %s | %s", read_msg[0], read_msg[1]);
        exit(0);
    }

    wait(&status);

    if(WIFEXITED(status)){
        printf("\nChild PID: %d | Exit value: %d",p ,WEXITSTATUS(status));
    }

    return 0;
}