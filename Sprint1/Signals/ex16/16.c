#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/syscall.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <sys/wait.h>

#define CHILDREN 50
#define SIMULATION_ONE_SUCCESS_PERCENTAGE 5
#define SIMULATION_TWO_SUCCESS_PERCENTAGE 100 - SIMULATION_ONE_SUCCESS_PERCENTAGE
#define SIGNAL_SUCCESS SIGUSR1
#define SIGNAL_INSUCCESS SIGUSR2
#define MASTER_SIGNAL SIGUSR1

volatile sig_atomic_t barrier;
volatile sig_atomic_t barrier2;
volatile sig_atomic_t master_handler;
volatile sig_atomic_t sigusr1_counter;
volatile sig_atomic_t sigusr2_counter;

void master_handle_SIGUSR1(int signo, siginfo_t *sinfo, void *context){
    char buffer[50];
    sigusr1_counter++;
    snprintf(buffer, 75, "SIGUSR1 counter: %d\n", sigusr1_counter);
    
    int buffer_length = strlen(buffer);
    write(STDOUT_FILENO, buffer, buffer_length);
}

void handle_SIGUSR2(int signo, siginfo_t *sinfo, void *context){
    char buffer[50];
    sigusr2_counter++;

    snprintf(buffer, 75, "SIGUSR2 counter: %d\n", sigusr2_counter);
    
    int buffer_length = strlen(buffer);
    write(STDOUT_FILENO, buffer, buffer_length);
}

void handler_act_alrm(int signo, siginfo_t *sinfo, void *context){
    //barrier2--;
}

void worker_handle_SIGUSR1(int signo, siginfo_t *sinfo, void *context){
    barrier2--;
    
    simulate2();
}

void handle_SIGINFO(int signo, siginfo_t *sinfo, void *context){
    barrier++;
}

int work_success(){
    time_t t;
    srand(getpid());
    int success = rand() % 100 + 1;
    //printf("Generated value: %d\n", success);
    if(success <= SIMULATION_ONE_SUCCESS_PERCENTAGE)
        return 1;
    else
        return 0;
}

int simulate1(){
    sleep(2);
    return work_success();
}

int simulate2(){
    sleep(2);
    return work_success();
}

int main(){

    sigusr1_counter = 0;
    sigusr2_counter = 0;
    barrier = 0;
    master_handler = 0;
    barrier2 = 1;


    // Handler para o sinal ALRM para que o pai notifique todos os filhos que estão à espera na linha 127.
    struct sigaction act_alrm;
    memset(&act_alrm, 0, sizeof(act_alrm));
    act_alrm.sa_sigaction = handler_act_alrm;
    sigaction(SIGALRM, &act_alrm, NULL);
    
    // Handler para o processo mestre tratar do sinal SIGUSR1 (incremento das simulações com resultado positivo).
    struct sigaction act_sigusr1;
    memset(&act_sigusr1, 0, sizeof(act_sigusr1));
    act_sigusr1.sa_sigaction = master_handle_SIGUSR1;
    sigemptyset(&act_sigusr1.sa_mask); // Supostamente, por defeito, o próprio sinal é bloqueado, por isso é, explicitamente, definido para não o fazer.
    sigaddset(&act_sigusr1.sa_mask, SIGCHLD);
    sigaction(SIGNAL_SUCCESS, &act_sigusr1, NULL);
        
    // Handler para o processo mestre tratar do sinal SIGUSR2 (incremento das simulações com resultado negativo).    
    struct sigaction act_sigusr2;
    memset(&act_sigusr2, 0, sizeof(act_sigusr2));
    act_sigusr2.sa_sigaction = handle_SIGUSR2;
    sigemptyset(&act_sigusr2.sa_mask);
    sigaddset(&act_sigusr2.sa_mask, SIGCHLD);
    sigaction(SIGNAL_INSUCCESS, &act_sigusr2, NULL);
    
    // Handler para o pai ser informado que todos os filhos já têm worker_handle_SIGUSR1 definido.
    struct sigaction act_info;
    memset(&act_info, 0, sizeof(act_info));
    act_info.sa_sigaction = handle_SIGINFO;
    sigaction(SIGCHLD, &act_info, NULL);
    
    // Array para guardar todos os PIDs.
    pid_t p[CHILDREN];
    int i;
    
    printf("\tPID: %d\n", getpid());

    for(i=0;i<CHILDREN;i++){
        p[i] = fork();
        if(p[i] == 0){
            
            // Handler dos processos filhos para o sinal SIGUSR1.
            memset(&act_sigusr1, 0, sizeof(act_sigusr1));
            act_sigusr1.sa_sigaction = worker_handle_SIGUSR1;
            //act_sigusr1.sa_flags = SA_SIGINFO; -> Usar o context para ver onde foi chamado.
            sigaction(MASTER_SIGNAL, &act_sigusr1, NULL);
            
            // O último processo filho vai enviar o sinal SIGCHLD para o pai para informar que todos os filhos já têm o handler configurado.
            if(i == CHILDREN-1){
                kill(getppid(), SIGCHLD);
            }else{
                // Todos os outros processos ficam à espera que o pai envie uma notificação para prosseguirem.
                pause();
            }

            int success = simulate1();
            
            if(success == 1){
                kill(getppid(), SIGNAL_SUCCESS);
            }else{
                kill(getppid(), SIGNAL_INSUCCESS);
            }
            
            /* 
            Após a execução de 25 processos existem processos que ainda não terminaram a simulação 1 e vão ser interrompidos para fazer a simulação 2,
            para não haver repetições de simulações, a variável barrier2 desincrementa de 1 para 0 na ocasião de entrar no SIGUSR1.
            */
            
            //(barrier2 == 1) ? printf("Vou executar simulate2 %d\n", getpid()) : printf("NÃO vou executar simulate2 %d\n", getpid());
            if(barrier2 == 1)
                simulate2();
            exit(EXIT_SUCCESS);
        }
    }

    if(i == CHILDREN){
        
        while(barrier != 1);
        i = 0;
        while(i < CHILDREN)
            kill(p[i++], SIGALRM);


        
        while(sigusr1_counter + sigusr2_counter < 25);
        int signal;
        if(sigusr1_counter == 0){
            printf("\nInefficient algorithm!\n\n");
            signal = SIGKILL;
        }else{
            printf("\nEfficient algorithm!\n\n");
            signal = MASTER_SIGNAL;
        }

        for(int j=0;j<CHILDREN;j++){
            kill(p[j], signal);
        }
    }
    for(int i=0;i<CHILDREN;i++){
        int status;
        wait(&status);
        if(WEXITSTATUS(status) != EXIT_SUCCESS){
            //printf("[%d]Saí com o erro: %d\n", (i+1), WEXITSTATUS(status));
        }else{
            //printf("[%d]Saí com o valor: %d\n", (i+1), WEXITSTATUS(status));
        }
    }
    
    printf("\n\nSIGUSR1: %d", sigusr1_counter);
    printf("\nSIGUSR2: %d", sigusr2_counter);

    exit(EXIT_SUCCESS); 
    // DÚVIDA: Se todos os processos filho saem garatidamente, o motivo da soma do counter usr1 e do counter usr2 não ser 50 é qual?
    // Não foi utilizado qualquer tipo de máscara, ou seja, todos os sinais, são tratados. Mesmo os que estão dentro do handler, são interrompidos e retomados.
    // Será algum tipo de informação que se perdeu nas interrupções do handler?
}