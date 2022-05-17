#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

void upcase(char *str) {
    while (*str) {
        *str = toupper(*str);
        str++;
    }
}

void handle_sig(int signo, siginfo_t *sinfo, void *context){

    sigset_t a_set;
    int signal;

    sigprocmask(SIG_BLOCK, NULL, &a_set);

    for (signal = 1; signal < NSIG; signal++) {
    		char *str = strdup(sys_siglist[signal]);
    		upcase(str);
    		write(STDOUT_FILENO, "Signal ", 7);
    		write(STDOUT_FILENO, str, sizeof(str));
            if (sigismember(&a_set,signal)==1) {
    			write(STDOUT_FILENO, " is blocked.\n", 13);
    		} else {
    			write(STDOUT_FILENO, " is not blocked.\n", 17);
    		}
    		free(str);
    	}
}

int main()
{
    sigset_t mask, pending;

    struct sigaction act;
    memset(&act, 0, sizeof(act)); // clear struct act

    sigfillset(&mask); // Blocks all signals.
    act.sa_sigaction = handle_sig;

    sigaction(SIGINT, &act, NULL);

    printf("PID=%d\n", getpid());
    pause();

    return 0;
}