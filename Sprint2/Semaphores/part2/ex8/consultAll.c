#include "info.h"

int main()
{

    sem_t *insert_semaphore = sem_open(SEMAPHORE_NAME_1, O_CREAT, 0644, SEMAPHORE_INITAL_VALUE_1);
    sem_t *consult_semaphore = sem_open(SEMAPHORE_NAME_2, O_CREAT, 0644, SEMAPHORE_INITAL_VALUE_0);

    int fd, data_size = sizeof(shared_data_t);
    shared_data_t *shared_data;
    fd = shm_open(SHARED_MEMORY_NAME, O_RDWR, S_IRUSR | S_IWUSR);
    ftruncate(fd, data_size);
    shared_data = (shared_data_t *)mmap(NULL, data_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    int total_clients = shared_data->number_of_clients;
    if (total_clients > 0)
    {
        sem_wait(consult_semaphore); // consult semaphore initial value is 0, so it will only continue for the first time when it is incremented by the insert program
        sem_wait(insert_semaphore);  // disables insertion while a consultation is happening

        for (int i = 0; i < total_clients; i++)
        {

            printf("Client's Number: %d \n", shared_data->info_array[i].number);
            printf("Client's Name: %s \n", shared_data->info_array[i].name);
            printf("Client's Address: %s \n", shared_data->info_array[i].address);
        }

        sem_post(consult_semaphore); // restores consult semaphore
        sem_post(insert_semaphore);  // restores insert semaphore
    }
    else
    {
        printf("The client list is empty \n");
    }

    return 0;
}