/*Write a program that creates 10 child processes. Each child process should print 100 numbers according to its
  creation order. That is the first child process created writes numbers 1..100, the second child process 101..200,
  the third 201..300, and so on. The parent process should wait for all child processes to finish.*/

#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

int main(){
    pid_t p;
    int begin = 1, end = 100;
    for(int i=0;i<10;i++){
        if(fork() == 0){
            for(int x=begin;x<=end;x++)
                printf("%d|", x);
            printf("--> [EXECUTED BY PID %d | CHILD %d]\n\n", getpid(), i+1);
            exit(0);
        }
        begin += 100;
        end += 100;
    }
    for(int i=0;i<10;i++)
        wait(NULL);

    return 0;
}