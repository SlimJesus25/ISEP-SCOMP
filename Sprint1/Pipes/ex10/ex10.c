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
#define MAX_RANGE 5
#define MIN_RANGE 1

int main(){

    int father_to_child[2], child_to_father[2];

    if(pipe(father_to_child) < 0){
        perror("Pipe");
        exit(EXIT_FAILURE);
    }

    if(pipe(child_to_father) < 0){
        perror("Pipe");
        exit(EXIT_FAILURE);
    }

    pid_t p = fork();

    if(p < 0){
        perror("Fork");
        exit(EXIT_FAILURE);
    }

    // Processos filhos.
    if(p == 0){
        time_t t;
        srand ((unsigned) time (&t));
        close(father_to_child[1]);
        close(child_to_father[0]);
        int credit = 0;
        while(5 == 5){
            int approval;
            read(father_to_child[0], &approval, sizeof(int));
            if(approval == 0){
                int quit = -1;
                write(child_to_father[1], &quit, sizeof(int));
                close(father_to_child[0]);
                close(child_to_father[1]);
                exit(EXIT_SUCCESS);
            }
            int bet = rand() % (MAX_RANGE - MIN_RANGE + 1) + MIN_RANGE;
            write(child_to_father[1], &bet, sizeof(int));
            
            read(father_to_child[0], &credit, sizeof(int));
            printf("CREDIT:%d\n", credit);
            sleep(1);
        }
    }

    int childHasCredit = 1;
    int child_credit = 20;
    int approval = 1;
    int not_approval = 0;

    while(childHasCredit){
        time_t t;
        srand ((unsigned) time (&t));
        if(child_credit >= 5){
            write(father_to_child[1], &approval, sizeof(int));
        }else{
            write(father_to_child[1], &not_approval, sizeof(int));
            close(father_to_child[1]);
            close(child_to_father[0]);
            break;
        }

        int random_value = rand() % (MAX_RANGE - MIN_RANGE + 1) + MIN_RANGE;
        int child_bet;

        read(child_to_father[0], &child_bet, sizeof(int));
        printf("\tChild bet: %d\n\tFather bet: %d\n", child_bet, random_value);
        if(child_bet == random_value){
            child_credit += 10;
        }else{
            child_credit -= 5;
        }
        
        write(father_to_child[1], &child_credit, sizeof(int));
    }
    
    printf("\n\n\tGAME OVER - NO CREDITS");

    exit(EXIT_SUCCESS);
}