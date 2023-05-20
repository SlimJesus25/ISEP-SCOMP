#include "info.h"

#define INSERT_PROG "./insert"
#define CONSULT_PROG "./consult"
#define CONSULT_ALL_PROG "./consultAll"

int main()
{

    printf("Welcome to X corp! \n \n");

    printf("To add personal records for one or more clients, please press '1' \n");
    printf("To consult a client's information by identification number press '2' \n");
    printf("To consult all clients information press '3'  \n");
    printf("To quit press '0' \n");

    int choice;
    scanf("%d", &choice);
    getchar(); // to consume the newline character left in the input buffer by scanf()

    while (choice != 0)
    {
        switch (choice)
        {
        case 1:

            execl(INSERT_PROG, "insert", NULL, NULL);
            break;

        case 2:

            execl(CONSULT_PROG, "consult", NULL, NULL);
            break;

        case 3:

            execl(CONSULT_ALL_PROG, "consultAll", NULL, NULL);
            break;

        default:
            printf("Invalid option, please try again (to quit press '0') \n");
            break;
        }

        scanf("%d", &choice);
        getchar(); // to consume the newline character left in the input buffer by scanf()
    }

    return 0;
}