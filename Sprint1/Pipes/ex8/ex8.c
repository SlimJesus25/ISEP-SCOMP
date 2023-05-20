#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/syscall.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <sys/wait.h>
#include <ctype.h>

#define CHILDREN 10

typedef struct{
    char win_message[4];
    int round;
}race_t;

int main(){

    race_t array[10];

    // Pipe comum a todos os futuros processos criados.
    if(pipe(fd) < 0){
        perror("Pipe");
        exit(EXIT_FAILURE);
    }

    for(int i=0;i<CHILDREN;i++){

        pid_t p = fork();

        if(p < 0){
        perror("Fork");
        exit(EXIT_FAILURE);
        }

        // Processos filhos.
        if(p == 0){
            close(fd[1]);
            race_t race;
            read(fd[0], &race, sizeof(race_t));
            printf("[%d]: %s\n", getpid(), race.win_message);
            exit(race.round);
        }
    }

    // Processo pai.
    for(int i=0;i<CHILDREN;i++){

        strcpy(array[i].win_message, "Win");
        array[i].round = i+1;

        write(fd[1], &array[i], sizeof(race_t));
        sleep(2);
    }

    printf("\n\n\t## Results ##");
    for(int i=0;i<CHILDREN;i++){
        int status = 0;
        pid_t winner = wait(&status);
        printf("Process %d, won round %d\n", winner, WEXITSTATUS(status));
    }



    exit(EXIT_SUCCESS);
}