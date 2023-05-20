#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/syscall.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/wait.h>
#include <ctype.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>

#define SHARED_MEMORY_NAME "/exercicio7"
#define SHARED_MEM_SIZE 3
#define CHILDREN 2
#define STR_SIZE 50
#define NR_DISC 10

struct aluno
{
    int numero;
    char nome[STR_SIZE];
    int disciplinas[NR_DISC];
    int valido;
};

int main()
{

    int fd, data_size = sizeof(struct aluno);

    struct aluno *shared_aluno;

    fd = shm_open(SHARED_MEMORY_NAME, O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR);

    if (fd < 0)
    {
        perror("shm_open");
        exit(EXIT_FAILURE);
    }

    if (ftruncate(fd, data_size) < 0)
    {
        perror("fruncate");
        exit(EXIT_FAILURE);
    }

    shared_aluno = (struct aluno *)mmap(NULL, data_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    if (shared_aluno == MAP_FAILED)
    {
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    pid_t p;
    int i;
    for (i = 0; i < CHILDREN; i++)
    {
        p = fork();
        if (p < 0)
        {
            perror("fork");
            exit(EXIT_FAILURE);
        }

        // Se for o filho vai sair do loop para salvaguardar o valor de i (útil para distinguir os filhos).
        if (p == 0)
        {
            break;
        }
    }
    // Filho 1.
    if (p == 0 && i == 0)
    {

        while (shared_aluno->valido == 0)
            ;

        int highest_grade = shared_aluno->disciplinas[0], lowest_grade = shared_aluno->disciplinas[0];

        for (int i = 1; i < NR_DISC; i++)
        {
            if (shared_aluno->disciplinas[i] > highest_grade)
            {
                highest_grade = shared_aluno->disciplinas[i];
            }

            if (shared_aluno->disciplinas[i] < lowest_grade)
            {
                lowest_grade = shared_aluno->disciplinas[i];
            }
        }

        printf("Highest grade: %d\n", highest_grade);
        printf("Lowest grade: %d\n", lowest_grade);

        exit(EXIT_SUCCESS);
    }

    // Filho 2.
    if (p == 0 && i == 1)
    {

        while (shared_aluno->valido == 0)
            ;

        double average_grade = shared_aluno->disciplinas[0];

        for (int i = 1; i < NR_DISC; i++)
        {
            average_grade += shared_aluno->disciplinas[i];
        }

        average_grade /= NR_DISC;

        printf("Average grade: %.2f\n", average_grade);

        exit(EXIT_SUCCESS);
    }

    char name_to_fill[STR_SIZE];

    printf("Nome do aluno: \n");
    fgets(name_to_fill, sizeof(name_to_fill), stdin);
    strcpy(shared_aluno->nome, name_to_fill);

    printf("\nNúmero: ");
    scanf("%d", &shared_aluno->numero);

    printf("Disciplinas:\n");
    for (int i = 0; i < NR_DISC; i++)
    {
        printf("\nDisc. %d: ", (i + 1));
        scanf("%d", &shared_aluno->disciplinas[i]);
    }

    printf("%s's stats \n", shared_aluno->nome);

    shared_aluno->valido = 1;

    for (int i = 0; i < CHILDREN; i++)
    {
        wait(NULL);
    }

    /*
    Disconecta a zona de memória partilhada do espaço do processo.
        void *addr -> Apontador para a zona de memória partilhada (retornado pelo mmap().
        size_t length -> Tamanho, em bytes, da zona de memória partilhada.
*/
    if (munmap(shared_aluno, data_size) < 0)
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
