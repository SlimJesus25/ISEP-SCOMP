#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>

/*
Given two integer arrays vec1 and vec2, with 1000 elements each, write a program that creates 5 child processes
to concurrently sum the two arrays. Each child should calculate vec1[i] + vec2[i] on 200 elements, sending all those
values to its parent. Ensure that each child computes different positions of the arrays. The parent process should
wait for all the 1000 values and then store them in a result array, in the correct position. Use 5 pipes, one for each
child.
*/

#define SIZE 1000

void sum_vectors(int *vec1, int *vec2, int min, int max){ // This function will make vec1 with the sum of both.
    for(int i=min;i<max;i++)
        vec1[i] += vec2[i];
}

int main(){

        int fd[5][2]; // File Descriptor with 5 lines.

        int vec1[SIZE], vec2[SIZE];

        int i;

        time_t t1;

        srand((unsigned) time (&t1));

        for(i=0;i<SIZE;i++){ // Fills both vectors with random numbers between [0, 200]
            vec1[i] = rand() % 200;
            vec2[i] = rand() % 200;
        }

        // For confirmation reasons, we will print 20 numbers from this vector to compare with the final vector.
        printf("%d + %d\n", vec1[10], vec2[10]);

        for(i=0;i<5;i++){ // Creation of 5 pipes
            if(pipe(fd[i]) == -1){
                perror("Pipe failed");
                return 1;
            }
        }

        pid_t p = fork();

        if(p < 0){
            perror("Failed creating child");
            return 1;
        }

        for(i=0;i<5;i++){
        /*
        The code inside of this repetition structure will produce 5 times,
        so that, 5 childs will be created and terminated in the final of their mission.
        */
            if(fork() == 0){ // If it is a child process, enters here and executes his mission.
                close(fd[i][0]);

                sum_vectors(vec1, vec2, i*200, (i+1)*200);

                write(fd[i][1], vec1, SIZE/5);
                close(fd[i][1]);

                exit(1);
            }
            /* Meanwhile child processes are doing their stuff, parent process is creating other childs processes and when all childs are created,
               parent process will advance in the code waiting for something to read from each child process.
             */
        }

        int sum_vec[5][SIZE/5];

        for(i=0;i<5;i++){
            close(fd[i][1]);
            read(fd[i][0], sum_vec[i], SIZE/5);
            close(fd[i][0]);
        }
        printf("\n\nResult\n");

        int sum_vec_od[SIZE], z=0;

        for(int x=0;x<5;x++){
            for(int y=0;y<SIZE/5;y++)
                sum_vec_od[z++] = sum_vec[x][y];
        }

        printf("%d\n", sum_vec_od[10]);

        /*for(i=0;i<SIZE;i++){
            if(sum_vec_od[i] != vec1[i]+vec2[i]){
                printf("Invalid");

            }
        }*/

    return 0;
}