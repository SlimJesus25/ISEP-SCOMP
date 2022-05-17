#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>


typedef struct{
    int productCode;
    char description[50];
    int price;
    int active_wait;
}product;

int main(){

    int fd, data_size = sizeof(product);

    product *ptr;

    fd = shm_open("/shrdmmry", O_CREAT|O_EXCL|O_RDWR, S_IRUSR|S_IWUSR);
    ftruncate(fd, data_size);

    ptr = (product *)mmap(NULL, data_size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);

    ptr->active_wait = 0;

    pid_t p = fork();

    if(p < 0){
        perror("Error creating process!");
        return EXIT_FAILURE;
    }

    if(p > 0){
        char product_description[] = "Produto gostoso";
        int product_code = 42069;
        int product_price=10;

        strcpy(ptr->description, product_description);
        ptr->productCode = product_code;
        ptr->price = product_price;


        ptr->active_wait = 1;
    }else{
        while(!ptr->active_wait);
        printf("Data in shared memory: %d | %s | %d\n",ptr->productCode, ptr->description, ptr->price);
    }

    shm_unlink("/shrdmmry");
    return 0;
}