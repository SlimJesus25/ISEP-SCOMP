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

#define SHARED_MEMORY_NAME "/exercicio12"
#define SHARED_MEM_SIZE 3
#define CHILDREN 1
#define STR_SIZE 50
#define NR_DISC 10

struct aluno{
    int numero;
    char nome[STR_SIZE];
    int disciplina[NR_DISC];
    int valido;
};


int main(){

    struct aluno a1;

    printf("\nNome: ");
    scanf(" %s", a1.nome);
    printf("\nNúmero: ");
    scanf("%d", &a1.numero);
    printf("\tDisciplinas\n");
    for(int i=0;i<NR_DISC;i++){
        printf("\nDisc. %d: ", (i+1));
        scanf("%d", &a1.disciplina[i]);
    }
    a1.valido = 1;
    
    
    

    int fd, size = sizeof(a1);
    struct aluno *addr;

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

    /* 
        Cria e abre uma zona de memória partilhada. 
            char* name -> Nome para dar à zona de memória partilhada (útil para ser utilizada por outros processos que não tenham qualquer relacionamento hierárquico).
            int oflag -> Definições de criação da zona de memória partilhada.
            mode_t mode -> Define permissões (estilo linux WRX).
    */
    fd = shm_open(SHARED_MEMORY_NAME, O_CREAT|O_EXCL|O_RDWR, S_IRUSR|S_IWUSR);
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
    addr = (struct aluno*)mmap(NULL, size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
    if(addr == MAP_FAILED){
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    addr->valido = 0;

    pid_t p = fork();
    if(p < 0){
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if(p == 0){

        while(addr->valido == 0);

        int highest_grade = addr->disciplina[0], lowest_grade = addr->disciplina[0];
        double average_grade = addr->disciplina[0];

        for(int i=1;i<NR_DISC;i++){

            if(addr->disciplina[i] > highest_grade){
                highest_grade = addr->disciplina[i];
            }

            if(addr->disciplina[i] < lowest_grade){
                lowest_grade = addr->disciplina[i];
            }

            average_grade += addr->disciplina[i];
        }

        average_grade /= NR_DISC;

        printf("%s's stats\n\n", addr->nome);
        printf("Highest grade: %d\n", highest_grade);
        printf("Lowest grade: %d\n", lowest_grade);
        printf("Average grade: %.2f\n", average_grade);

        exit(EXIT_SUCCESS);
    }

    addr->numero = a1.numero;
    strcpy(addr->nome, a1.nome);
    for(int i=0;i<NR_DISC;i++)
        addr->disciplina[i] = a1.disciplina[i];
    addr->valido = 1;

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