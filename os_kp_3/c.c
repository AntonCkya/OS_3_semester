#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>

#define True 1

int fdAC, fdCB;
/*  A ---string---> C
    C ---number---> B  */

int main(){

    fdAC = open("tempAC", O_RDWR);
    fdCB = open("tempCB", O_RDWR | O_CREAT | O_TRUNC, 0777);

    if (fdAC < 0 || fdCB < 0){
		printf("Error while opening a file\n");
		return 0;
	}

    ftruncate(fdCB, 2 * sizeof(int));

    char *ptrAC;
    int *ptrCB;
    ptrAC = mmap(NULL, 1025 * sizeof(char), PROT_WRITE | PROT_READ, MAP_SHARED, fdAC, 0);
    ptrCB = mmap(NULL, 2 * sizeof(int), PROT_WRITE | PROT_READ, MAP_SHARED, fdCB, 0);
    /*
    ptr[0] = 0 - готово к записи
    ptr[0] = 1 - запись произошла, можно читать
    ptr[0] = 2 - запись окончена, a.k.a. данных больше не будет
    */

    if (ptrCB == MAP_FAILED || ptrAC == MAP_FAILED){
        printf("Error while memory map created\n");
        return 0;
	}

    ptrCB[0] = 0;
    printf("Programm C started\n");

    char string[1024];
    int count = 0;
    while (True){
        while (ptrAC[0] == 0){
            // активное ожидание
        }
        if (ptrAC[0] == 1){
            while(ptrAC[count + 1] != '\n'){
                string[count] = ptrAC[count + 1];
                count++;
            }
            ptrAC[0] = 0;
            printf("C:");
            for (int i = 0; i < count; i++){
                printf("%c", string[i]);
            }
            printf("\n");
            ptrCB[1] = count;
            ptrCB[0] = 1;
            count = 0;
        } else if (ptrAC[0] == 2){
            break;
        }
    }
    
    if (close(fdCB) == -1 || close(fdAC) == -1){
		printf("Error while closing a files\n");
		return 0;
    }

    munmap(ptrCB, 2 * sizeof(int));
    munmap(ptrAC, 1025 * sizeof(char));

    return 0;
}