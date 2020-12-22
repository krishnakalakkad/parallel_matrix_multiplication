#include <sys/mman.h>
#include <sys/wait.h>
#include <sys/stat.h> 
#include <fcntl.h> 
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAP_ANON 0x20

void print_matrix(float* m)//for 32*32
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


int main(int argc, char *argv[]){

    int n;


    sscanf(argv[2], "%d", &n);

    int fd1 = shm_open("matrixA", O_RDWR | O_CREAT, 0777);
    ftruncate(fd1, 1024*sizeof(float));
    float *A = (float *)mmap(NULL, 1024*sizeof(float), PROT_READ | PROT_WRITE, MAP_SHARED, fd1, 0);

    int fd2 = shm_open("matrixB", O_RDWR | O_CREAT, 0777);
    ftruncate(fd2, 1024*sizeof(float));
    float *B = (float*)mmap(NULL, 1024*sizeof(float), PROT_READ | PROT_WRITE, MAP_SHARED, fd2, 0);

    int fd3 = shm_open("matrixC", O_RDWR | O_CREAT, 0777);
    ftruncate(fd3, 1024*sizeof(float));
    float *C = (float*)mmap(NULL, 1024*sizeof(float), PROT_READ | PROT_WRITE, MAP_SHARED, fd3, 0);

    int fd4 = shm_open("ready", O_RDWR | O_CREAT, 0777);
    ftruncate(fd4, n*sizeof(int));
    int *ready = (int*)mmap(NULL, n*sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, fd4, 0);

    

    char processNum[3];

    char *programName = (char *)malloc(100);

    char *args[3];

    args[0] = (char *)malloc(100);
    args[1] = (char *)malloc(100);
    args[2] = NULL;
    

    

    strcpy(args[1], argv[2]);
    
    strcpy(programName, argv[1]);



    

    for(int i = 0; i < n; i++){
        if(!fork()){
            sprintf(processNum, "%d", i);
            strcpy(args[0], processNum);
            execv(programName, args);
            return 0;
        }
        sleep(1);
    }
    wait(0);

    close(fd1);
    shm_unlink("matrixA");
    munmap(A, 1024*sizeof(float));

    close(fd2);
    shm_unlink("matrixB");
    munmap(B, 1024*sizeof(float));

    close(fd3);
    shm_unlink("matrixC");
    munmap(C, 1024*sizeof(float));

    close(fd4);
    shm_unlink("ready");
    munmap(ready, n*sizeof(int));

    free(programName);
    free(args[0]);
    free(args[1]);




    
    return 0;
    
}
