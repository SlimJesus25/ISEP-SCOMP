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
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <math.h>
#include <pthread.h>

void* print_prime_number(void *arg){
    
    int value = *((int*)arg);

    if(value >= 1)
        print_prime_number_r(1, value);

    pthread_exit((void*)index);
}

void print_prime_number_r(int i, int value){
    if(i >= value)
        return;
    if(i % 2 != 0)
        printf("%d\n", i);
    print_prime_number_r(i+2, value);
}

int main(){
    
    int* number = (int*)calloc(1, sizeof(int));
    printf("Enter a number: ");
    scanf("%d", number);

    pthread_t thread;

    pthread_create(&thread, NULL, print_prime_number, (void*)number);
    
    pthread_join(thread, NULL);

    pthread_exit((void*)NULL);

}