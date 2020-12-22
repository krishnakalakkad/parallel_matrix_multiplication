#include <sys/mman.h>
#include <sys/stat.h> 
#include <sys/wait.h>

#include <fcntl.h> 
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>


void print_matrix(float* m)//for 4*4
    {
    printf("_______________________________________________________\n");
    for (int r = 0; r < 32; r++)
        {
        for (int c = 0; c < 32; c++)
            printf("%.1f  ", m[c + r * 32]);            
        printf("\n");
        }
    printf("_______________________________________________________\n");
    }

void synch(int par_id,int par_count,int *ready){
    int synchid = ready[par_count]+1; 
    ready[par_id]=synchid;
    int breakout = 0;
    while(1){
        breakout=1;
        for(int i=0;i<par_count;i++){ 
            if(ready[i]<synchid){
                breakout = 0;
                break;
            }
        }
        if(breakout==1) {
            ready[par_count] = synchid;
            break;
        } 
    }
}


void matrixMultiply(int processNum, int numLines, float *A, float *B, float *C){
    int start = processNum * numLines;
    float dotProduct;
    for(int i = start; i < (start + numLines); i++)
        for(int j = 0; j < 32; j++){
            dotProduct = 0;
            for (int k = 0; k < 32; k++)
                dotProduct += A[32*i + k] * B[j + 32*k];
            C[32 * i + j] = dotProduct; 
        }
}

int main(int argc, char *argv[]){


    int numLines, processNum, n;
    float dotProduct;

    sscanf(argv[0], "%d", &processNum);
    sscanf(argv[1], "%d", &n);

    
    srand(time(0)); 

    numLines = 32/ n;

    int fd1 = shm_open("matrixA", O_RDWR | O_CREAT, 0777);
    if(!processNum)
        ftruncate(fd1, 1024*sizeof(float));
    float *A = mmap(NULL, 1024*sizeof(float), PROT_READ | PROT_WRITE, MAP_SHARED, fd1, 0);

    int fd2 = shm_open("matrixB", O_RDWR | O_CREAT, 0777);
    if (!processNum)
        ftruncate(fd2, 1024*sizeof(float));
    float *B = mmap(NULL, 1024*sizeof(float), PROT_READ | PROT_WRITE, MAP_SHARED, fd2, 0);

    int fd3 = shm_open("matrixC", O_RDWR | O_CREAT, 0777);
    if(!processNum)
        ftruncate(fd3, 1024*sizeof(float));
    float *C = mmap(NULL, 1024*sizeof(float), PROT_READ | PROT_WRITE, MAP_SHARED, fd3, 0);

    int fd4 = shm_open("ready", O_RDWR | O_CREAT, 0777);
    if (!processNum)
        ftruncate(fd4, n*sizeof(int));
    int *ready = (int *)mmap(NULL, n*sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, fd4, 0);




    if(!processNum){

        for(int i = 0; i < 1024; i++){
            float x = (float)rand()/(float)(RAND_MAX/10.0);
            A[i] = x;
        }

        for(int i = 0; i < 1024; i++){
            float x = (float)rand()/(float)(RAND_MAX/10.0);
            B[i] = x;
        }

    }


    clock_t a = clock();
    matrixMultiply(processNum,32/n, A,B,C);
    clock_t b = clock();
    synch(processNum, n, ready);
    if(!processNum){
        print_matrix(C);
        printf("time taken: %lf sec\n", (double)(b-a)/CLOCKS_PER_SEC);
    }
        
    a = clock();
    matrixMultiply(processNum, 32/n, B,C,A);
    b = clock();
    synch(processNum, n, ready);
    if(!processNum){
        print_matrix(A);
        printf("time taken: %lf sec\n", (double)(b-a)/CLOCKS_PER_SEC);
    }
    a = clock();
    matrixMultiply(processNum, 32/n, A,C,B);
    b = clock();
    synch(processNum, n, ready);
    if(!processNum){
        print_matrix(B);
        printf("time taken: %lf sec\n", (double)(b-a)/CLOCKS_PER_SEC);
    }



    



    return 0;
}
