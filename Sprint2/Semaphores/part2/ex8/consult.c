#include "info.h"

int main()
{

    sem_t *insert_semaphore = sem_open(SEMAPHORE_NAME_1, O_CREAT, 0644, SEMAPHORE_INITAL_VALUE_1);
    sem_t *consult_semaphore = sem_open(SEMAPHORE_NAME_2, O_CREAT, 0644, SEMAPHORE_INITAL_VALUE_0);

    int fd, data_size = sizeof(shared_data_t), choice = 1, found_clients;
    shared_data_t *shared_data;

    fd = shm_open(SHARED_MEMORY_NAME, O_RDWR, S_IRUSR | S_IWUSR);
    ftruncate(fd, data_size);
    shared_data = (shared_data_t *)mmap(NULL, data_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    do
    {

        sem_wait(consult_semaphore); // consult semaphore initial value is 0, so it will only continue when it is incremented by the insert program
        sem_wait(insert_semaphore);  // disables insertion while a consultation is happening

        printf("\nPlease input the specific client's identification number \n");
        int number_to_find;
        scanf("%d", &number_to_find);
        getchar(); // to consume the newline character left in the input buffer by scanf()

        int total_clients = shared_data->number_of_clients;

        if (total_clients > 0)
        {
            found_clients = 0;
            for (int i = 0; i < total_clients; i++)
            {

                if (shared_data->info_array[i].number == number_to_find)
                {
                    printf("Client's information: \n");
                    printf("Client's Number: %d \n", shared_data->info_array[i].number);
                    printf("Client's Name: %s \n", shared_data->info_array[i].name);
                    printf("Client's Address: %s \n", shared_data->info_array[i].address);

                    found_clients++;
                }
            }
            if (found_clients == 0)
            {
                printf("Client number not found \n");
            }
        }
        else
        {
            printf("The client list is empty \n");
        }

        printf("\nTo search another client's information by identification number type '1' \n");
        printf("To exit press '0' \n \n");
        scanf("%d", &choice);
        getchar(); // to consume the newline character left in the input buffer by scanf()

        sem_post(consult_semaphore); // restores consult semaphore
        sem_post(insert_semaphore);  // restores insert semaphore

    } while (choice == 1);

    return 0;
}