#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/wait.h>
/*
Write a program that creates a child process and establishes with it a communication channel through a pipe. The
parent process starts by printing its PID and then sends it to its child through the pipe. The child process should
read the parent ́s PID from the pipe and print it.
*/

#define BUFFER_SIZE 1000;

int main(){

    int read_msg;
    int write_msg;
    int fd[2];

    if(pipe(fd) == -1){
        perror("Pipe failed!");
        return 1;
    }

    if(fork() > 0){
        write_msg = getpid();
        printf("\n[PARENT] PID: %d", write_msg);
        close(fd[0]);
        write(fd[1], &write_msg, sizeof(write_msg));
        close(fd[1]);
    }else{
        close(fd[1]);
        read(fd[0], &read_msg, sizeof(ssize_t));
        close(fd[0]);
        printf("\n[CHILD] PID received from the pipe: %d\n", read_msg);
        exit(0);
    }

    wait(NULL);

    return 0;

}