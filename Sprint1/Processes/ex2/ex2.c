#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PHRASES_NUMBER 3
int main(){
    
    char phrases[3][8] = {"I'm...\n", "the...\n", "father!\n"};

	char child_phrase[30] = "I'll never join you!\n";


	for(int i=0;i<PHRASES_NUMBER;i++){
		printf(" %s", phrases[i]);
		if(fork() == 0){
			printf(" %s", child_phrase);
			exit(EXIT_SUCCESS);
		}
		wait(NULL);
		
	}

	return 0;
}
