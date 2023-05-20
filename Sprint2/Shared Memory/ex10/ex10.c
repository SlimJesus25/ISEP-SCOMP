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

#define SHARED_MEMORY_NAME "/exercicio9"
#define SHARED_MEM_SIZE 10
#define CHILDREN 10
#define NUMBER_OF_EXCHANGES 30
#define ARRAY_SIZE 10
#define WRITE 1
#define READ 0


typedef struct{
    int values[ARRAY_SIZE];
    int values_size;
    int valid;
}prod_t;

int main(){

    int fd, size = SHARED_MEM_SIZE*sizeof(prod_t);
    prod_t *addr;

    /*
                ## FLAGS ##
        O_CREAT -> Cria, mas se já existir vai utilizar.
        O_EXCL -> Cria (em conjunto com a flag O_CREAT) e dá erro se já existir.
        O_TRUNC -> Elimina qualquer conteúdo existente na memória partilhada.
        O_RDWR -> Abre para leitura e escrita.
        O_RDONLY -> Abre para leitura.

                ## MODES ##
        Define permissões de leitura/escrita/execução para utilizadores/grupos/outros.
        S_IRUSR -> Permissão de leitura. 
        S_IWUSR -> Permissão de escrita.
        S_IXUSR -> Permissão de execução.
    */

   // Pipes para consumidor e produtor.
    int cons_to_prod[2], prod_to_cons[2];
    if(pipe(cons_to_prod) < 0 && pipe(prod_to_cons) < 0){
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    srand(time(NULL));

    for(int i=0;i<SHARED_MEM_SIZE;i++){
        int r = rand() % 100;
        write(cons_to_prod[WRITE], &r, sizeof(int));
    }

    /* 
        Cria e abre uma zona de memória partilhada. 
            char* name -> Nome para dar à zona de memória partilhada (útil para ser utilizada por outros processos que não tenham qualquer relacionamento hierárquico).
            int oflag -> Definições de criação da zona de memória partilhada.
            mode_t mode -> Define permissões (estilo linux WRX).
    */
    fd = shm_open(SHARED_MEMORY_NAME, O_CREAT|O_RDWR, S_IRUSR|S_IWUSR);
    if(fd < 0){
        perror("shm_open");
        exit(EXIT_FAILURE);
    }


    /*
        Define o tamanho da área e inicializa a zero.
            int fd -> Descritor de ficheiros (Utilizar o retorno da função shm_open).
            off_t length -> Tamanho, em bytes, da memória.
    */
    if(ftruncate(fd, size) < 0){
        perror("ftruncate");
        exit(EXIT_FAILURE);
    }

    /*
        Mapeia a zona de memória partilhada no espaço do processo.
            void *addr -> Define o endereço específico para armazenar (no processo) a zona de memória partilhada. Nota: Se for usado NULL, o SO decide automaticamente.
            size_t length -> Define o tamanho necessário para ser usado.
            int prot -> Flags de proteção. PROT_READ: ler apenas e PROT_READ|PROT_WRITE: ler e escrever. Nota: Deve ser coerente com o shm_open.
            int flags -> Controla o comportamento do mmap(). Nota: No caso de memória partilhada, utiliza-se sempre MAP_SHARED para permitir outros processos verem mudanças.
            int fd -> Descritor de ficheiros retornado por shm_open().
            off_t offset -> Inicio do mapeamento na área da memória. Nota: Normalmente é zero para usar a memória toda desde o início.
    */
    addr = (prod_t*)mmap(NULL, size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
    if(addr == MAP_FAILED){
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    // Criação do consumidor e do produtor.
    pid_t p = fork();
    if(p < 0){
        perror("fork");
        exit(EXIT_FAILURE);
    }

    // Consumidor.
    if(p == 0){
        int values[SHARED_MEM_SIZE];

        int index=0, iterator=0;

        while(iterator++ < NUMBER_OF_EXCHANGES){

            close(prod_to_cons[WRITE]);
            close(cons_to_prod[READ]);

            // Aguarda por permissão do consumidor à zona crítica.
            read(prod_to_cons[READ], &index, sizeof(int));
            printf("\n[CONSUMIDOR] Permissão concedida... (Índice %d)\n", index);
            fflush(stdout);

            printf("\n[CONSUMIDOR] Valores lidos\n");
            fflush(stdout);
            // Lê informação da zona de memória partilhada.
            for(int i=0;i<index;i++){
                values[i] = addr->values[i];
                printf("%da|", addr->values[i]);
            }
            printf("\n");

            // Escreve para o produtor o valores recebidos.
            for(int i=0;i<index;i++){
                write(cons_to_prod[WRITE], values+i, sizeof(int));
            }
        }


        exit(EXIT_SUCCESS);
    }

    // Produtor.
    int values[SHARED_MEM_SIZE];

    int index=SHARED_MEM_SIZE, iterator=0;

    while(iterator++ < NUMBER_OF_EXCHANGES){

        printf("\n[PRODUTOR] Valores lidos do consumidor...\n");
        fflush(stdout);
        close(cons_to_prod[WRITE]);
        close(prod_to_cons[READ]);
        // Lê valores do consumidor.
        for(int i=0;i<index;i++){
            read(cons_to_prod[READ], values+i, sizeof(int));
            printf("%db|", values[i]);
        }
        fflush(stdout);
        // Escreve na memória partilhada.
        for(int i=0;i<index;i++){
            addr->values[i] = values[i];
        }

        // Dá permissão ao consumidor para ler da memória partilhada.
        write(prod_to_cons[WRITE], &index, sizeof(int));
        printf("\n[PRODUTOR] Concedida a permissão ao CONSUMIDOR...\n");
        fflush(stdout);
    }

    wait(NULL);


        /*
        Disconecta a zona de memória partilhada do espaço do processo.
            void *addr -> Apontador para a zona de memória partilhada (retornado pelo mmap().
            size_t length -> Tamanho, em bytes, da zona de memória partilhada.
    */
    if(munmap(addr, size) < 0){
        perror("munmap");
        exit(EXIT_FAILURE);
    }

    /*
        Fecha o descritor de ficheiros retornado pelo shm_open().
            int fd -> Descritor de ficheiros delvolvido pelo shm_open().
    */
    if(close(fd) < 0){
        perror("close");
        exit(EXIT_FAILURE);
    }

    /*
        Remove a zona de memória partilhada do sistema de ficheiros. Marca-a para ser apagada, mal todos os processos que, eventualmente, a estejam a usar, fechem.
        Nota: Não é instantâneo! Se algum processo estiver a usar a zona de memória, a memória vai ser marcada para ser apagada, mal consiga!
            const char* name -> Nome da zona de memória partilhada.
    */
    if(shm_unlink(SHARED_MEMORY_NAME) < 0){
        perror("shm_unlink");
        exit(EXIT_FAILURE);
    }
    

    exit(EXIT_SUCCESS);
}