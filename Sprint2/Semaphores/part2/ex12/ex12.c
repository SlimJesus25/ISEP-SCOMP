#include "info.h"

sem_t *mutex, *can_write, *can_read;

void producer(circular_buffer_t *buff, int start, int end)
{
    for (int i = start; i < end; i++)
    {

        sem_wait(can_write);
        sem_wait(mutex);

        buff->data[buff->tail] = i + 1;
        printf("Producer pid: %d, wrote: %d (pos = %d) \n ", getpid(), buff->data[buff->tail], buff->tail);
        // buff->tail = (buff->tail + 1) % BUFFER_SIZE;
        buff->head = buff->tail;
        buff->tail++;

        sem_post(mutex);
        sem_post(can_read);

        sleep(1);
    }
}

void consumer(circular_buffer_t *buff)
{

    for (int i = 0; i < TOTAL_INTEGERS; i++)
    {

        sem_wait(can_read);
        sem_wait(mutex);

        printf("Consumer read: %d (pos = %d) \n", buff->data[buff->head], buff->head);
        // buff->head = (buff->head + 1) % BUFFER_SIZE;
        buff->data[buff->head] = 0;
        buff->tail--;
        buff->head--;

        sem_post(mutex);
        sem_post(can_write);
        sleep(2);
    }
}

int main()
{
    mutex = sem_open(SEMAPHORE_MUTEX, O_CREAT | O_EXCL, 0644, SEMAPHORE_INITAL_VALUE_1);
    can_write = sem_open(SEMAPHORE_WRITE, O_CREAT | O_EXCL, 0644, BUFFER_SIZE);
    can_read = sem_open(SEMAPHORE_READ, O_CREAT | O_EXCL, 0644, SEMAPHORE_INITAL_VALUE_0);

    if ((mutex == SEM_FAILED) || (can_write == SEM_FAILED) || (can_read == SEM_FAILED))
    {
        perror("sem_open");
        exit(EXIT_FAILURE);
    }

    int fd, data_size;

    pid_t p;

    circular_buffer_t *shared_data;
    data_size = sizeof(circular_buffer_t);

    fd = shm_open(SHARED_MEMORY_NAME, O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR);

    if (fd < 0)
    {
        perror("shm_open");
        exit(EXIT_FAILURE);
    }

    if (ftruncate(fd, data_size) < 0)
    {
        perror("ftruncate");
        exit(EXIT_FAILURE);
    }

    shared_data = (circular_buffer_t *)mmap(NULL, data_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    if (shared_data == MAP_FAILED)
    {
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    shared_data->head = 0;
    shared_data->tail = 0;

    for (int i = 0; i < CHILD_NUM; i++)
    {
        p = fork();

        if (p < 0)
        {
            perror("fork 1");
            exit(EXIT_FAILURE);
        }
        else if (p == 0)
        {
            p = fork();
            if (p < 0)
            {
                perror("fork 2");
                exit(EXIT_FAILURE);
            }
            if (p > 0) // child 1
            {
                producer(shared_data, 0, (TOTAL_INTEGERS / 2));
            }
            else if (p == 0) // child 2
            {
                producer(shared_data, (TOTAL_INTEGERS / 2), TOTAL_INTEGERS);
                exit(EXIT_SUCCESS);
            }
            exit(EXIT_SUCCESS);
        }
        else if (p > 0)
        {
            consumer(shared_data);
        }
    }

    // closing everything

    if (munmap(shared_data, data_size) < 0)
    {
        perror("munmap");
        exit(EXIT_FAILURE);
    }

    if (close(fd) < 0)
    {
        perror("close");
        exit(EXIT_FAILURE);
    }
    if (shm_unlink(SHARED_MEMORY_NAME) < 0)
    {
        perror("shm_unlink");
        exit(EXIT_FAILURE);
    }

    if ((sem_unlink(SEMAPHORE_MUTEX) < 0) || (sem_unlink(SEMAPHORE_READ) < 0) || (sem_unlink(SEMAPHORE_WRITE) < 0))
    {
        perror("sem_unlink");
        exit(EXIT_FAILURE);
    }

    return 0;
}