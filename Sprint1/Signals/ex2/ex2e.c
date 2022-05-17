#include <stdio.h>
#include <signal.h>
#include <stdlib.h>

int main() {
	int signal;
	pid_t pid;

	printf("\nSignal:");
	scanf(" %d",&signal);

	printf("\nProcess ID:");
	scanf("%d",&pid);

	kill(pid,signal);

	printf("Done!");

}