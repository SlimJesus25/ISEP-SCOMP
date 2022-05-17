/*Write a program that creates two child processes, the father waits for each one of them to terminate and then
outputs their exit value. The first child process created should sleep for 1 second and then exit with a return value
of 1. The second child created should sleep for 2 seconds and exit returning 2.*/

#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

int main(void){
    pid_t p1_wait, p1 = fork();
    int status_child_one;

    if(p1 < 0){
        perror("Error creating child process!");
        return 1;
    }

    if(p1 > 0){
        pid_t p2_wait, p2 = fork();
        int status_child_two;
        if(p2 < 0){
            perror("Error creating child 2 process!");
            return 1;
        }
        if(p2 > 0){
            p1_wait = waitpid(p1, &status_child_one, 0);
            if(WIFEXITED(status_child_one))
                printf("\nChild 1: %d", WEXITSTATUS(status_child_one));

            p2_wait = waitpid(p2, &status_child_two, 0);
            if(WIFEXITED(status_child_two))
                printf("\nChild 2: %d", WEXITSTATUS(status_child_two));

        }else if(p2 == 0){
            sleep(2);
            exit(2);
        }
    }else if(p1 == 0){
        sleep(1);
        exit(1);
    }

    return 0;
}