#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

int spawn_childs(int n);

int main(void){
    pid_t pai_id = getpid();


    int n_childs = 6;
    int index = spawn_childs(n_childs);
    if(pai_id == getpid()){
    for(int i=0; i<n_childs; i++){
        wait(NULL);
    }
   } else {
    int exit_value=index*2;
    exit(exit_value);
   }

   printf("Cya Sons");
   return 0; 
}

int spawn_childs(int n){
    pid_t pai_id =getpid();
    int id;

    for(int i=0;i<n;i++){
        pid_t pid = getpid();
        if(pai_id == pid){
            id=0;
            if(fork()==0){
            id=n;
        }
        }
    }
    return id;
}