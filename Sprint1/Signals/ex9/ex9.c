#include <stdio.h>
#include <unistd.h>

int main(){
    for(;;){
        printf("I like signals\n");
        sleep(1);
    }
}