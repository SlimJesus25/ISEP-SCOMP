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

#define CHILDREN 10
#define MAX_VALUE_TO_GENERATE 500000
#define MIN_VALUE_TO_GENERATE 1
#define MAX_SIZE 50000

typedef struct{
    int costumer_code;
    int product_code;
    int quantity;
}sales_t;

void generate_random(sales_t* array, int max_size, int max_value_to_generate, int min_value_to_generate){
    for (int i = 0; i < max_size; i++){
        array[i].costumer_code = rand () % (max_value_to_generate - min_value_to_generate + 1) + min_value_to_generate;
        array[i].product_code = rand () % (max_value_to_generate - min_value_to_generate + 1) + min_value_to_generate;
        array[i].quantity = rand () % max_value_to_generate;
    }
}

sales_t search_product_quantity(sales_t* array, int quantity, int min_limit, int max_limit){
    for(int i=min_limit;i<max_limit;i++){
        if(array[i].quantity >= quantity){
            return array[i];
        }
    }
    sales_t error;
    error.costumer_code = 0;
    error.product_code = 0;
    error.quantity = 0;
    return error;
}

int main(){

    time_t t;
    srand ((unsigned) time (&t));

    sales_t array[MAX_SIZE];

    generate_random(array, MAX_SIZE, MAX_VALUE_TO_GENERATE, MIN_VALUE_TO_GENERATE);
    
    // for(int i=0;i<20;i++){
    //     printf("CC: %d|PC: %d|Q: %d\n\n", array[i].costumer_code, array[i].product_code, array[i].quantity);
    // }
    
    int fd[2];
    
    // Pipe comum a todos os futuros processos criados.
    if(pipe(fd) < 0){
        perror("Pipe");
        exit(EXIT_FAILURE);
    }
    
    int quant;
    printf("\n");
    printf("Quantity to search: ");
    scanf("%d", &quant);
    
    printf("\n\n");
    
    for(int i=0;i<CHILDREN;i++){
        pid_t p = fork();
        if(p < 0){
        perror("Fork");
        exit(EXIT_FAILURE);
        }
        
        if(p == 0){
            int exit_value = EXIT_SUCCESS;
            close(fd[0]);
            int min = i*(MAX_SIZE/CHILDREN);
            int max = (i+1)*(MAX_SIZE/CHILDREN);
            
            sales_t valid_product = search_product_quantity(array, quant, min, max);
            if(valid_product.product_code == 0)
                exit_value = EXIT_FAILURE;
            write(fd[1], &valid_product, sizeof(sales_t));

            close(fd[1]);
            exit(exit_value);
        }
    }
    
    sales_t* products_array;
    close(fd[1]);
    
    int count=0, incre=0;
    for(int i=0;i<CHILDREN;i++){
        sales_t valid_sale;
        read(fd[0], &valid_sale, sizeof(sales_t));
        
        // Na ocasião do filho enviar algo com um código 0, é porque não foi encontrado nenhum registo com a quantidade maior ou igual a "quant".
        if(valid_sale.product_code == 0 || valid_sale.costumer_code == 0){
            continue;
        }
        products_array = (sales_t*)realloc(products_array, (incre+1)*sizeof(sales_t));
        products_array[incre].product_code = valid_sale.product_code;
        products_array[incre].costumer_code = valid_sale.costumer_code;
        products_array[incre++].quantity = valid_sale.quantity;
    }
    
    close(fd[0]);
    
    for(int i=0;i<CHILDREN;i++){
        wait(NULL);
    }
    
    for(int i=0;i<incre;i++){
        printf("\t%dº Sales received\nCustomer code: %d\nProduct code: %d\nQuantity: %d\n\n", ++count, products_array[i].costumer_code, products_array[i].product_code, products_array[i].quantity);
    }

    exit(EXIT_SUCCESS);
}