#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[]){
    if(argc > 0)
    printf(" -> Sou o programa \"%s\"\n", argv[0]);
    sleep(10);
    printf(" -> Execução \"%s\" acabada!\n\n", argv[0]);
    return 0;
}