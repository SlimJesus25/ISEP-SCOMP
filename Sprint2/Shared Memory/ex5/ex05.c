#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <time.h>
#include <signal.h>

#define INITIAL_VALUE 100
#define ITERATIONS 1000000
#define SHARED_MEMORY_NAME "/exercício5"
#define SIZE 1
#define STR_SIZE 22

void handle_USR1(int signo, siginfo_t *sinfo, void *context)
{
    write(STDOUT_FILENO, "\nSent Signal to father \n", STR_SIZE);
}

int main()
{
    struct sigaction act;

    memset(&act, 0, sizeof(struct sigaction));
    sigemptyset(&act.sa_mask);
    act.sa_sigaction = handle_USR1;
    act.sa_flags = SA_SIGINFO;

    sigaction(SIGUSR1, &act, NULL);

    pid_t p;

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

    int fd, data_size = SIZE * sizeof(int);

    int *addr;

    fd = shm_open(SHARED_MEMORY_NAME, O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR);

    if (fd < 0)
    {
        perror("shm_open");
        exit(EXIT_FAILURE);
    }

    /*
        Define o tamanho da área e inicializa a zero.
            int fd -> Descritor de ficheiros (Utilizar o retorno da função shm_open).
            off_t length -> Tamanho, em bytes, da memória.
    */

    if (ftruncate(fd, data_size) < 0)
    {
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

    addr = (int *)mmap(NULL, data_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    if (addr == MAP_FAILED)
    {
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    // initializing the variable
    *(addr) = INITIAL_VALUE;

    p = fork();
    if (p < 0)
    {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (p == 0)
    {
        for (int i = 0; i < ITERATIONS; i++)
        {
            *(addr) += 1;
            *(addr) -= 1;
        }
        kill(getppid(), SIGUSR1);
    }
    else if (p > 0)
    {
        pause();
        for (int i = 0; i < ITERATIONS; i++)
        {
            *(addr) += 1;
            *(addr) -= 1;
        }
    }

    if (p == 0)
    {
        printf("\n[CHILDREN] Final value: %d\n", *(addr));
        exit(EXIT_SUCCESS);
    }

    printf("[FATHER] Final value: %d\n", *(addr));

    /*
       Disconecta a zona de memória partilhada do espaço do processo.
           void *addr -> Apontador para a zona de memória partilhada (retornado pelo mmap().
           size_t length -> Tamanho, em bytes, da zona de memória partilhada.
   */
    if (munmap(addr, data_size) < 0)
    {
        perror("munmap");
        exit(EXIT_FAILURE);
    }

    /*
            Fecha o descritor de ficheiros retornado pelo shm_open().
                int fd -> Descritor de ficheiros delvolvido pelo shm_open().
        */
    if (close(fd) < 0)
    {
        perror("close");
        exit(EXIT_FAILURE);
    }
    /*
       Remove a zona de memória partilhada do sistema de ficheiros. Marca-a para ser apagada, mal todos os processos que, eventualmente, a estejam a usar, fechem.
       Nota: Não é instantâneo! Se algum processo estiver a usar a zona de memória, a memória vai ser marcada para ser apagada, mal consiga!
           const char* name -> Nome da zona de memória partilhada.
   */

    if (shm_unlink(SHARED_MEMORY_NAME) < 0)
    {
        perror("shm_unlink");
        exit(EXIT_FAILURE);
    }
}