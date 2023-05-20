#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

int main(){

    int sinal;

    pid_t p;

    printf("\nSinal: ");
    scanf("%d", &sinal);

    printf("\nPID: ");
    scanf("%d", &p);

    kill(p, sinal);


    return 0;
}