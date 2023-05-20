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
#include <semaphore.h>
#include "../../semaphore_utils.h"

#define MUTEX_GENERIC_NAME "/mutex"
#define SYNC_GENERIC_NAME "/sync"
#define SHARED_MEMORY_NAME "/shared_memory"
#define SERVER 0
#define CLIENT 1
#define MEASUREMENTS 6
#define SENSORS 5

typedef struct{
    int measurement_values[MEASUREMENTS];
    int alarm_state;
    int last_modification_sensor;
    int next_index;
}sensor_t;

/*
        ## Explicação ##
        Para resolver este exercício, a solução foi inspirada no algoritmo do Sleeping Barber (Server-Client).
        O controlador é um servidor e os sensores os seus clientes. Em cada iteração dos sensores, são atualizados dados 
        e o controlador é notificado para poder fazer o print das novas infomações. 
        Existem 2 semáforos de sincronização, um para os clientes desbloquearem o servidor (um de cada vez) e outro para
        o servidor desbloquear os clientes. Existem também um semáforo de exclusão mútua para assegurar o acesso exclusivo
        na zona de memória partilhada.
    
*/

int main(){

    
    sem_t* sv_cli[2];
    create_semaphores(sv_cli, 2, SYNC_GENERIC_NAME, SYNCHRONIZATION_INITAL_VALUE);

    sem_t* mutex = sem_open(MUTEX_GENERIC_NAME, O_CREAT|O_EXCL, 0644, MUTEX_INITAL_VALUE);

    if(mutex == SEM_FAILED){
        perror("sem_open");
        exit(EXIT_FAILURE);
    }
    
    // Criação de uma memória partilhada com espaço para cada um dos sensores colocar o respetivo valor, estado do alarme e o seu índice.
    int fd, size = SENSORS*sizeof(sensor_t);
    sensor_t *addr;

    fd = shm_open(SHARED_MEMORY_NAME, O_CREAT|O_EXCL|O_RDWR, S_IRUSR|S_IWUSR);
    if(fd < 0){
        perror("shm_open");
        exit(EXIT_FAILURE);
    }

    if(ftruncate(fd, size) < 0){
        perror("ftruncate");
        exit(EXIT_FAILURE);
    }

    addr = (sensor_t*)mmap(NULL, size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
    if(addr == MAP_FAILED){
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    // Criação de clientes.
    int i;
    for(i=0;i<SENSORS;i++){
        pid_t p = fork();
        if(p < 0){
            perror("fork");
            exit(EXIT_FAILURE);
        }

        // Processos sensor.
        if(p == 0){
            srand(getpid());

            for(int j=0;j<MEASUREMENTS;j++){

                sem_wait(mutex);

                // Atribuição do valor aleatório (0-100).
                int r = rand() % 100;
                
                // Atualiza o estado do sensor (alarme -> normal) caso as condições sejam satisfeitas.
                if(j > 0){
                    if(r <= 50 && (addr+i)->measurement_values[j-1] <= 50){
                        (addr+i)->alarm_state = 0;
                    }
                }
                
                // Atualiza o estado do sensor (normal -> alarme) caso as condições sejam satisfeitas.
                if(r > 50 && (addr+i)->alarm_state == 0){
                    (addr+i)->alarm_state = 1;
                }

                // Atualização dos valores medidos no vetor de medições do respetivo sensor.
                (addr+i)->measurement_values[j] = r; 

                // Indicação do sensor que fez as últimas atualizações.
                addr->last_modification_sensor = i;

                // Indicação do próximo índice relativamente a um sensor.
                (addr+i)->next_index += 1;

                sem_post(sv_cli[CLIENT]);
                sem_wait(sv_cli[SERVER]);
                sem_post(mutex);
                sleep(1);
            }

            exit(EXIT_FAILURE);
            }
        }

    int previous_alarm_counter = 0;

    for(int i=0;i<MEASUREMENTS*SENSORS;i++){

        // De 0 a 5 -> índice 0.
        // De 6 a 11 -> índice 1.
        // ...
        int j = i/MEASUREMENTS;
    
        // Espera pelo incremento de um sensor.
        sem_wait(sv_cli[CLIENT]);
        
        // Sensor que fez a última atualização.
        int index = (addr)->last_modification_sensor;

        // Índice alterado no vetor de medições.
        int measurement_index = ((addr+index)->next_index)-1;

        // Valor alterado.
        int received_value = (addr+index)->measurement_values[measurement_index];

        // Estado do sensor.
        int alarm_state = addr->alarm_state;
        
        // Contagem de sensores em estado alarme.
        int alarm_counter=0;
        for(int k=0;k<SENSORS;k++){
            if((addr+k)->alarm_state == 1){
                alarm_counter++;
            }
        }
        
        // Valor recebido na iteração (do respetivo sensor) anterior.
        int previous_received_value = -1;
        
        if(measurement_index > 0){
            previous_received_value = (addr+index)->measurement_values[measurement_index-1];
        }
        
        printf("Value %d received from sensor %d...\n", received_value, index);

        
        if((addr+index)->alarm_state == 0 && received_value <= 50 && previous_received_value <= 50 && previous_received_value >= 0 && alarm_counter != previous_alarm_counter){
            printf("Since the value measured was %d and the previous was %d, the alarm is now in normal state...\n", received_value, previous_received_value);
            previous_alarm_counter--;
        }
        
        if(alarm_counter != previous_alarm_counter){
            previous_alarm_counter = alarm_counter;
            printf("%d sensors in alarm state!\n", alarm_counter);
        }
        
        printf("\n\n");

        // Incrementa um sensor.
        sem_post(sv_cli[SERVER]);
    }

    for(int i=0;i<SENSORS;i++){
        wait(NULL);
    }

    unlink_semaphores(SYNC_GENERIC_NAME, 2);
    if(sem_unlink(MUTEX_GENERIC_NAME) < 0){
        perror("sem_unlink");
        exit(EXIT_FAILURE);
    }

    if(munmap(addr, size) < 0){
        perror("munmap");
        exit(EXIT_FAILURE);
    }

    if(close(fd) < 0){
        perror("close");
        exit(EXIT_FAILURE);
    }

    if(shm_unlink(SHARED_MEMORY_NAME) < 0){
        perror("shm_unlink");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}