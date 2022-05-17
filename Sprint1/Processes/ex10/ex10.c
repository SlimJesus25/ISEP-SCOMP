#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

int main(void)
{

   int n=100;  // Numero que eu procuro
   int sz=2000;
   int min=0;
   int max=200;
   int randArray[sz];
   for(int i=0;i<sz;i++){
      randArray[i]=rand()%100; //gera um numero random entre 0 e 99
  }

  for(int i=0; i<10;i++){
    if(fork()==0){
        
        for(int j=min;j<=max;j++){
            if(n == randArray[j]){
                exit(j);
            }

        }
        exit(255);
    }

    min +=200;
    max += 200;
  }



    for(int k=1; k<11;k++){
        int status;
        wait(&status);
        printf("Resultado: %d \n",WEXITSTATUS(status));   //O resultado nunca vai ser o valor real do indice além do primeiro e até ao 255 do 2º intervalo pois as restantes posições  
}                                                         //Excede o primeiro byte que o valor status armazena do exit()       


   return 0; 
}