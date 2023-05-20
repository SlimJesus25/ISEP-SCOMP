#include "info.h"

int main()
{

    sem_t *insert_semaphore = sem_open(SEMAPHORE_NAME_1, O_CREAT, 0644, SEMAPHORE_INITAL_VALUE_1);
    sem_t *consult_semaphore = sem_open(SEMAPHORE_NAME_2, O_CREAT, 0644, SEMAPHORE_INITAL_VALUE_0);

    int fd, data_size = sizeof(shared_data_t), choice, client_number;

    char client_name[STR_SIZE], client_address[STR_SIZE];

    shared_data_t *shared_data;

    fd = shm_open(SHARED_MEMORY_NAME, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);

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

    shared_data = (shared_data_t *)mmap(NULL, data_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    if (shared_data == MAP_FAILED)
    {
        perror("mmap");
        exit(EXIT_FAILURE);
    }
    int total_clients = shared_data->number_of_clients;

    printf("\nTo add a client's personal record press '1' \n");
    printf("\nTo exit press '0' \n");

    scanf("%d", &choice);
    getchar(); // to consume the newline character left in the input buffer by scanf()

    while (choice != 0)
    {
        if (choice == 1)
        {
            sem_wait(insert_semaphore); // decrements the insert semaphore, 1 => 0

            if (total_clients > 0)
            {
                sem_wait(consult_semaphore);
            }

            char client_number_str[STR_SIZE];
            printf("Please insert the client's name: \n");
            fgets(client_name, STR_SIZE, stdin);

            printf("Please insert the client's address: \n");
            fgets(client_address, STR_SIZE, stdin);

            printf("Please insert the client's number: \n");
            fgets(client_number_str, STR_SIZE, stdin);
            sscanf(client_number_str, "%d", &client_number);

            snprintf(shared_data->info_array[total_clients].name, STR_SIZE, "%s", client_name);
            snprintf(shared_data->info_array[total_clients].address, STR_SIZE, "%s", client_address);
            shared_data->info_array[total_clients].number = client_number;

            shared_data->number_of_clients += 1;

            sem_post(insert_semaphore);  // increments the insert semaphore 0 => 1
            sem_post(consult_semaphore); // increments the consult semaphore making to possible to consult the clients
        }

        printf("\nTo add another client's personal records type '1' \n");
        printf("\nTo exit type '0' \n");
        scanf("%d", &choice);
        getchar(); // to consume the newline character left in the input buffer by scanf()
    }

    return 0;
}