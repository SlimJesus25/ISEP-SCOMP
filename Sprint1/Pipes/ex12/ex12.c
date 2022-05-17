#include <stdio.h>
#include <string.h>
#include <unistd.h>	   
#include <sys/types.h> 
#include <stdlib.h>	   

#define N_PRODUTOS 5



int main(){
	
	struct struct1{
		char nome[50];
		float preco;
		int cod_barras;
	};
	
	struct struct1 base_dados[N_PRODUTOS];
	strcpy(base_dados[0].nome, "Farinha de aveia");
	base_dados[0].preco = 0.69;
	base_dados[0].cod_barras = 1;
	strcpy(base_dados[1].nome, "Grey Goose");
	base_dados[1].preco = 129.99;
	base_dados[1].cod_barras = 2;
	strcpy(base_dados[2].nome, "Redbull");
	base_dados[2].preco = 2.00;
	base_dados[2].cod_barras = 3;
	strcpy(base_dados[3].nome, "Teste Covid");
	base_dados[3].preco = 4.10;
	base_dados[3].cod_barras = 4;
	strcpy(base_dados[4].nome, "Tofu");
	base_dados[4].preco = 0.79;
	base_dados[4].cod_barras = 5;

	int fd[6][2], i, j, indice_, exst;
	int cod_barras;
	
	for(i = 0; i < 6; i++){
		if(pipe(fd[i]) == -1){
			perror("Pipe arriou");
			return 1;
		}
	}
	
	for(i = 0; i < 5; i++){
		if(fork() == 0){
			close(fd[0][0]);
			close(fd[i+1][1]);
			printf("Barcode Reader %d - Produtos(1, 2, 3, 4, 5)\n", i+1);
			scanf("%d", &cod_barras);
			write(fd[0][1], &cod_barras, sizeof(int));
			write(fd[0][1], &i, sizeof(int));
			struct struct1 prod;
			read(fd[i+1][0], &prod, sizeof(struct struct1));
			printf("Barcode Reader %d - Nome: %s\nPreco: %.2f euros\n", i+1, prod.nome, prod.preco);
			close(fd[0][1]);
			close(fd[i+1][0]);
			exit(0);
		}
	}

	for(i = 0; i < 5; i++){
		close(fd[0][1]);
		close(fd[i+1][0]);
		read(fd[0][0], &cod_barras, sizeof(int));
		read(fd[0][0], &indice_, sizeof(int));
		exst = 0;
		
		for(j = 0; j < N_PRODUTOS; j++){
			if(cod_barras == base_dados[j].cod_barras){
				write(fd[indice_+1][1], &base_dados[j], sizeof(struct struct1));
				exst = 1;
			}
		}
		
		if(exst == 0){
			struct struct1 prod;
			strcpy(prod.nome, "Não existe esse produto na base de dados");
			prod.preco = 0;
			write(fd[indice_+1][1], &prod, sizeof(struct struct1));
		}
	}

	return 0;
}