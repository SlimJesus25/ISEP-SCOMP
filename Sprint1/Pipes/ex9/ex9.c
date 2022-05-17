#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>

/*
A retail company stores the sales of the previous month in a sales array and wants to know which products have
sold more than 20 units in a single sale. As the array size is quite large (50 000 elements) a concurrent solution to
the problem is necessary. Write a program that creates 10 child processes, with each one responsible for searching
5000 sale records.

Whenever a child finds a product that meets the search criteria it should send the product’s code to the parent
process. The parent process should record those codes in the array products, printing it whenever all child processes
have been terminated. The values on the sales array should be filled with random values.
*/

#define SIZE 50000

typedef struct{
    int customer_code;
    int product_code;
    int quantity;
} sales;

int sales_search(sales *arr, int min, int max){
    for(int i=min;i<max;i++){
        if((arr+i)->quantity > 20)
            return (arr+i)->product_code;
    }
    return -1;
}

int main(){

    sales vec[SIZE];

    srand(time(NULL));

    for(int i=0;i<SIZE;i++){
        vec[i].customer_code = i;
        vec[i].product_code = i+10;
        vec[i].quantity = rand() % 30;
    }

    int fd[10][2];

    for(int i=0;i<10;i++){
        if(pipe(fd[i]) < 0){
            perror("Pipe failed!");
            return 1;
        }
    }


    for(int i=0;i<10;i++){
        if(fork() == 0){
            close(fd[i][0]);
            int code_product = sales_search(vec, i*5000, (i+1)*5000);

            write(fd[i][1], &code_product, sizeof(int));
            close(fd[i][1]);
            exit(1);
        }
    }

    int large_sold_products_ids[SIZE];

    for(int i=0;i<10;i++){
        close(fd[i][1]);
        read(fd[i][0], large_sold_products_ids+i, sizeof(int));
        close(fd[i][0]);
    }
    printf("\nProduct that sold more than 20 units (ID)\n");
    for(int i=0;i<10;i++){
        if(large_sold_products_ids[i] >= 0)
            printf("|%d|", large_sold_products_ids[i]);
    }

    return 0;
}