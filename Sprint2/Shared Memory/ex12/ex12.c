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

#define STR_SIZE 50
#define NR_DISC 5

typedef struct{
    int numero;
    char nome[STR_SIZE];
    int disciplinas[NR_DISC];
    int active_wait;
}aluno;

int main(){
    int fd;
    int data_size=sizeof(aluno);

    aluno *ptr;

    fd = shm_open("/sh12", O_CREAT|O_EXCL|O_RDWR, S_IRUSR|S_IWUSR);
    ftruncate(fd, data_size);

    ptr = mmap(NULL, data_size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);

    ptr->active_wait=1;



    pid_t p = fork();

    if(p < 0){
        perror("Error creating process!");
        return EXIT_FAILURE;
    }

    if(p>0){
    printf("Numero: ");
    scanf("%d",&ptr->numero);

    printf("Nome: ");
    scanf("%s",ptr->nome);

    printf("Nota 1: ");
    scanf("%d",&ptr->disciplinas[0]);

    printf("Nota 2: ");
    scanf("%d",&ptr->disciplinas[1]);

    printf("Nota 3: ");
    scanf("%d",&ptr->disciplinas[2]);

    printf("Nota 4: ");
    scanf("%d",&ptr->disciplinas[3]);

    printf("Nota 5:");
    scanf("%d",&ptr->disciplinas[4]);

    ptr->active_wait=0;

    } else {
        while(ptr->active_wait);
        int average;
        for(int i=0;i<NR_DISC;i++){
            average+=ptr->disciplinas[i];
        }

        average=average/NR_DISC;

        printf("The student %s number %d as a grade average of %d",ptr->nome,ptr->numero,average);
         exit(1);

    }
    
    wait(NULL);
    shm_unlink("/sh12");

    return 0;
}