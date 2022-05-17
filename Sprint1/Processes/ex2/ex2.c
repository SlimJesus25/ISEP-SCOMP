#include<stdio.h>
#include<sys/types.h>
#include<unistd.h>
#include<stdlib.h>


int main(){

	pid_t p = fork();
	if(p==0){
		printf("I'll never join you!\n");
		exit(1);
	} else {
		printf("I'm...\n");
	}


	p = fork(); 
	
	if(p==0){
		printf("I'll never join you!\n");
		exit(1);
	} else {
		printf("the...\n");
	}

	p = fork(); 
	if(p==0){
		printf("I'll never join you!\n");
		exit(1);
	} else {
		printf("father!\n");
	}
	return 0;
}