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

#define SHARED_MEMORY_NAME "/exercicio8"
#define SHARED_MEM_SIZE 10
#define PATH_LENGTH 100
#define WORD_LENGTH 20
#define CHILDREN 10
#define INFO_FILE "./info.txt"

typedef struct
{
    char path_to_file[PATH_LENGTH];
    char word_to_search[WORD_LENGTH];
    int occurrence;
} info_t;

// Corta os caracteres especiais de uma string.
int trim_special_chars(char *str, int len)
{
    int i, j;
    int new_len = len;
    for (i = j = 0; i < len; i++)
    {
        if ((str[i] >= 33 && str[i] <= 47) || (str[i] >= 58 && str[i] <= 64))
        {
            str[j++] = str[i];
            new_len--;
        }
    }

    return new_len;
}

int main()
{

    int fd, size = SHARED_MEM_SIZE * sizeof(info_t);
    info_t *addr;

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
    addr = (info_t *)mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (addr == MAP_FAILED)
    {
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    // Ponteiro para ficheiro de informação.
    FILE *fptr;
    fptr = fopen(INFO_FILE, "r");

    if (fptr == NULL)
    {
        perror("fopen 1");
        exit(EXIT_FAILURE);
    }

    char buf[32];

    // Leitura dos ficheiros e das palavras a procurar.
    for (int i = 0; i < CHILDREN; i++)
    {
        fscanf(fptr, "%s", buf);
        strcpy((addr + i)->path_to_file, buf);
        fscanf(fptr, "%s", buf);
        strcpy((addr + i)->word_to_search, buf);
    }

    // Criação de processos para pesquisar em cada ficheiro as respetivas palavras.
    for (int i = 0; i < CHILDREN; i++)
    {
        pid_t p = fork();
        if (p < 0)
        {
            perror("fork");
            exit(EXIT_FAILURE);
        }

        if (p == 0)
        {

            FILE *ptr;

            ptr = fopen((addr + i)->path_to_file, "r");

            if (ptr == NULL)
            {
                perror("fopen 2");
                exit(EXIT_FAILURE);
            }

            char buffer[32];
            do
            {
                if (fscanf(ptr, "%s", buffer) == EOF)
                {
                    break;
                }

                trim_special_chars(buffer, strlen(buffer) + 1);

                if (buffer == NULL)
                {
                    perror("realloc");
                    exit(EXIT_FAILURE);
                }

                if (strcmp(buffer, (addr + i)->word_to_search) == 0)
                {
                    (addr + i)->occurrence = (addr + i)->occurrence + 1;
                }

            } while (1 == 1);

            exit(EXIT_SUCCESS);
        }
    }

    for (int i = 0; i < CHILDREN; i++)
    {
        wait(NULL);
    }

    printf("\n\tEstatísticas\n\n");

    for (int i = 0; i < CHILDREN; i++)
    {
        printf("Ficheiro: %s | Palavra: %s | Ocorrências: %d\n",
               (addr + i)->path_to_file, (addr + i)->word_to_search, (addr + i)->occurrence);
    }

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