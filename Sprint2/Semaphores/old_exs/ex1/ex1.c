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

#define SEMAPHORE1_NAME "/sem_mutex_1__"
#define SEMAPHORE2_NAME "/sem_mutex_2__"
#define SEMAPHORE_INITAL_VALUE 1

#define NUMBER_PER_CHILDREN 200
#define CHILDREN 8
#define NUMBERS_FILE "Numbers.txt"
#define OUTPUT_FILE "Output.txt"

int main()
{

    printf("\n");

    /*
        Cria ou abre um semáforo existente.
         - sem_t *sem_open(const char* name, int oflag, mode_t mode, unsigned int value);
         - sem_t *sem_open(const char* name, int oflag);
        A variante com mais dois parâmetros é para abrir um existente.
        Retorna o endereço do semáforo em caso de sucesso, ou -1 (SEM_FAILED) em caso de erro.

                ## FLAGS ##
            O_CREAT -> Cria o semáforo, caso não exista.
            O_EXCL -> Cria (juntamente com o O_CREAT) se não existir, mas dá erro na ocasião de já existir.

                ## MODES ##
            Os modes são iguais para todas as funções, desde que sejam como as de Linux.
            Os modes estão mencionados abaixo, na shared memory.


            const char* name -> Nome do semáforo.
            int oflag -> Definições de criação do semáforo.
            mode_t mode -> Permissões, tal como as de Linux.
            value -> Valor inicial do semáforo.
    */
    sem_t *semaphore_1 = sem_open(SEMAPHORE1_NAME, O_CREAT | O_EXCL, 0644, SEMAPHORE_INITAL_VALUE);
    sem_t *semaphore_2 = sem_open(SEMAPHORE2_NAME, O_CREAT | O_EXCL, 0644, SEMAPHORE_INITAL_VALUE);

    /**
     * Esta função incrementa o valor atual do semáforo passado por parâmetro.
     * sem_post(sem_t* semaphore)
     *
     * Esta função, similar à de cima, vai desincrementar o valor atual do semáforo passado por parâmetro.
     * sem_wait(sem_t* semaphore)
     *
     * NOTA: A funçaõ sem_wait, retorna imediatamente um valor (de erro (se for -1) ou de sucesso (se for 0)) mal tenha hipótese de desincrementar.
     * Na ocasião de não conseguir, vai bloquear o(s) processo(s) que estiver(em) a aguardar.
     *
     * As seguintes funções são semelhantes a sem_wait, porém mais específicas.
     *
     * Vai tentar bloquear, porém não vai bloquear caso não consiga desincrementar. Se não for possível desincrementar, retorna um erro.
     * sem_trywait(sem_t* semaphore)
     *
     * Existe ainda outra variante que vai permitir que um processo bloqueie (à espera que tenha permissão para desincrementar o semáforo),
     * mas apenas até ao valor especificado por parâmetro.
     * sem_timedwait(sem_t* semaphore, const struct timespec* abs_timeout)
     *
     * Esta função vai obter o valor atual do semáforo. O segundo parâmetro, sval, vai ser o endereço onde o valor vai ser guardado.
     * sem_getvalue(sem_t* semaphore, int *sval)
     *
     */

    if (semaphore_1 == SEM_FAILED && semaphore_2 == SEM_FAILED)
    {
        perror("sem_open");
        exit(EXIT_FAILURE);
    }

    printf("A\n");

    for (int i = 0; i < CHILDREN; i++)
    {

        pid_t p = fork();

        if (p < 0)
        {
            perror("fork");
            exit(EXIT_FAILURE);
        }

        // Zona dos processos filhos.
        if (p == 0)
        {
            /*
                Tendo em conta que o problema pede por um semáforo de exclusão mútua,
                é colocado o valor inicial a um permitindo que apenas um processo leia do ficheiro "Numbers.txt".
            */
            printf("A1\n");
            sem_wait(semaphore_1);
            printf("A2\n");

            FILE *fptr = fopen(NUMBERS_FILE, "r");
            printf("Abri o ficheiro\n");

            if (fptr == NULL)
            {
                perror("fopen 1");
                exit(EXIT_FAILURE);
            }

            // Variável que vai armazenando os valores do ficheiro.
            char number[10];
            // Array de inteiros que vai armazenar os valores a escrever no ficheiro "Output.txt".
            int read_values[NUMBER_PER_CHILDREN], i = 0;

            while (fscanf(fptr, "%s", number) != EOF && i < NUMBER_PER_CHILDREN)
            {
                read_values[i++] = atoi(number);
            }

            fclose(fptr);

            // Volta a permitir que um processo novo entre neste excerto de código e o que já cá estava sai.
            printf("A3\n");
            sem_post(semaphore_1);

            printf("A4\n");
            sem_wait(semaphore_2);

            FILE *fptr_w = fopen(OUTPUT_FILE, "a");

            if (fptr_w == NULL)
            {
                perror("fopen 2");
                exit(EXIT_FAILURE);
            }

            for (int i = 0; i < NUMBER_PER_CHILDREN; i++)
            {
                fprintf(fptr_w, "[%d] %d\n", getpid(), read_values[i]);
            }

            fclose(fptr_w);

            sem_post(semaphore_2);
            exit(EXIT_SUCCESS);
        }
    }
    for (int i = 0; i < CHILDREN; i++)
    {
        wait(NULL);
    }

    FILE *fptr = fopen(OUTPUT_FILE, "r");

    if (fptr == NULL)
    {
        perror("fopen 3");
        exit(EXIT_FAILURE);
    }

    char output[50];
    int i = 0;
    while (fscanf(fptr, "%s", output) != EOF)
    {
        printf("%s", output);
        if (i++ % 2 != 0)
        {
            printf("\n");
        }
    }

    // Remove o semáforo.
    // const char* name -> Nome do semáforo a remover.
    if (sem_unlink(SEMAPHORE1_NAME) < 0)
    {
        perror("sem_unlink 1");
    }

    if (sem_unlink(SEMAPHORE2_NAME) < 0)
    {
        perror("sem_unlink 2");
    }

    exit(EXIT_SUCCESS);
}