#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

#define CHILDS 4

void main(){
    int i;
    int status;
    pid_t p;

    for (i = 0; i < CHILDS; i++) {
        p = fork();
        if (p == 0) {
            sleep(1); /*sleep(): unistd.h*/
            exit(EXIT_SUCCESS);
        }
    }
    printf("This is the end.\n");
}