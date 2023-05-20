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

#define SHARED_MEMORY_NAME "/exercicio5"
#define SIZE 1
#define LOOP_ITERATIONS 1000000

void handle_USR1(int signo, siginfo_t *sinfo, void *context)
{
    switch (signo)
    {
    case SIGUSR1:
        // FATHER
        *(addr) += 1;
        *(addr) -= 1;
        printf("Aqui 1: %d\n", *(addr));
        kill(p, SIGUSR2);
        break;

    case SIGUSR2:
        // CHILD
        pause();
        *(addr) += 1;
        *(addr) -= 1;
        printf("Aqui 2: %d\n", *(addr));
        kill(getppid(), SIGUSR1);
    default:
        break;
    }
    // Vazio, serve apenas para não terminar o processo.
}

// Código do writer.

int main()
{

    int fd, size = SIZE * sizeof(int);
    int *addr;

    struct sigaction act;

    memset(&act, 0, sizeof(struct sigaction));
    sigemptyset(&act.sa_mask);
    act.sa_sigaction = handle_USR1;
    act.sa_flags = SA_SIGINFO;

    sigaction(SIGUSR1, &act, NULL);
    sigaction(SIGUSR2, &act, NULL);

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
    fd = shm_open(SHARED_MEMORY_NAME, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
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
    if (ftruncate(fd, size) < 0)
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
    addr = (int *)mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (addr == MAP_FAILED)
    {
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    // Inicializando os valor inicial na memória partilhada.
    *(addr) = 100;

    pid_t p = fork();
    if (p < 0)
    {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    int i = 0;
    while (i++ < LOOP_ITERATIONS)
    {

        if (p == 0)
        {
        }
        else
        {
            pause();
        }
    }

    if (p == 0)
    {
        printf("[CHILDREN] Final value: %d\n", *(addr));
        exit(EXIT_SUCCESS);
    }

    printf("[FATHER] Final value: %d\n", *(addr));

    /*
    Disconecta a zona de memória partilhada do espaço do processo.
        void *addr -> Apontador para a zona de memória partilhada (retornado pelo mmap().
        size_t length -> Tamanho, em bytes, da zona de memória partilhada.
*/
    if (munmap(addr, size) < 0)
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

    exit(EXIT_SUCCESS);
}