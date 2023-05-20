#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <time.h>

int spawn_childs(int n){
    pid_t p;
    for(int i=0;i<n;i++){
        p = fork();
        if(p == 0){
            return i+1;
        }
    }
    return 0;
}

int main(){

    int index = spawn_childs(4);
    printf("Index value: %d\n", index);

    return 0;
}